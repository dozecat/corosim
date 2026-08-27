![Language](https://img.shields.io/badge/Language-C%2B%2B20-f34b7d.svg) ![Simulation](https://img.shields.io/badge/Simulation-Verilator-007ec6.svg) ![License](https://img.shields.io/badge/License-MIT-yellow.svg)

[English](#en) | [中文](#cn)

---

<span id="en">corosim</span>
===========================

**corosim** is a C++20 coroutine-based co-simulation engine for [Verilator](https://www.veripool.org/verilator/). It maps `co_await` to delta-cycle and timed-event scheduling, letting you write concurrent hardware testbenches without manual state machines or eval loops.

## Features

- **Coroutine concurrency** — `co_await posedge()`, `co_await delay()`, `co_await any()`
- **Non-blocking signals** — `sig.next(val)` with delta-cycle scheduling
- **Trigger-based processes** — `sim.always()` for repeating triggers, `sim.instance()` for one-shot coroutines
- **Phase-aligned I/O** — `sim.sample()` / `sim.drive()` for BFM-style transactions
- **Software signals** — cross-process sync without DUT binding

## Quick Start

### 1. Install Dependencies
```bash
# macOS (with Homebrew)
brew install verilator

# Ubuntu/Debian
sudo apt-get install verilator g++ make
```

### 2. Clone and Run
```bash
git clone https://github.com/dozecat/corosim.git
cd corosim/examples/async_fifo/sim/tb

make        # Compile and run simulation
```

### 3. View Waveform
```bash
gtkwave waveform.vcd
```

## Project Structure

```
corosim/
├── src/                        # Library source
│   ├── corosim.hpp             # Umbrella header
│   ├── api/                    # Public facades: Simulator<TOP>, Module, Dut
│   ├── core/                   # Kernel, types, detail (engine core)
│   ├── signal/                 # SignalVal, Signal<T>, wide, registry
│   ├── trigger/                # Trigger specs + delay/any awaiters
│   ├── coroutine/              # Task, Coroutine, WaitGroup, CoroutineManager
│   └── scheduler/              # Delta-cycle + timed event engine
└── examples/
    ├── async_fifo/             # Dual-clock async FIFO (basic)
    └── axis_async_fifo/        # AXI4-Stream async FIFO (advanced, with BFM)
```

## API Reference

### Simulator&lt;TOP&gt; — User-Facing Facade

Most testbenches interact through `Simulator<TOP>`, which owns a `Kernel` and provides signal binding, coroutine registration, and clock generation:

```cpp
#include "corosim.hpp"
using namespace corosim;

Vasync_fifo top;
Simulator sim(top);                  // wrap the Verilator top

auto& clk = sim.sig(top.clk);         // bind a DUT signal
auto& rst = sim.sig<uint8_t>();       // software-only signal (no DUT)

sim.clock(clk, 10);                   // toggle clk every 5 time units

sim.run(500, [&](sim_time t) { tfp.dump(t); });
```

For pointer-to-member binding, use `Dut<TOP>`:
```cpp
Dut<TOP> dut(top, sim.signals());
auto& clk = dut.sig(&TOP::clk);
auto& rst = dut.sig(&TOP::rst);
```

### Signal — Verilator Field Binding

```cpp
Signal<uint8_t>  sig(&top->sig_field);  // bind to a Verilator signal
Signal<bool>     flag;                  // software-only (no binding)

sig.next(val);    // non-blocking assignment (applied at delta end)
sig.read();       // read current value
auto v = sig;     // implicit read

// Mapping from Verilator types:
Signal<uint8_t>  rst(&top.rst);     // 1-bit   → CData
Signal<uint8_t>  data(&top.data);   // 8-bit   → CData
Signal<uint16_t> wide(&top.wide);   // 16-bit  → SData
```

For wide vectors (`VlWide<N>`), use `Signal<VlWide<N>>` — see `signal/wide.hpp`.

### Triggers

| Trigger | Type constraint | `sim.always()` | `co_await` |
|---------|----------------|:---:|:---:|
| `posedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `negedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `change(sig)` | any `Signal<T>` | ✓ | ✓ |
| `delay(n)` | — | ✓ | ✓ |

`posedge`/`negedge` require 1-byte signal types; `change` works with any signal width.

### Processes

```cpp
// Auto-repeat on trigger
sim.always(posedge(clk), [&] { cnt.next(cnt.read() + 1); });
sim.always(delay(5), [&] { clk.next(!clk.read()); });

// One-shot coroutine (lambda)
sim.instance([&]() -> Task {
    co_await delay(30);
    go.next(true);
});

// One-shot coroutine (standalone function)
sim.instance(reset, &rst, &wr_clk);
```

Coroutine functions support `co_await` for synchronization:

```cpp
Task reset(Signal<uint8_t>* rst, Signal<uint8_t>* clk) {
    rst->next(1);
    for (int i = 0; i < 5; i++) co_await posedge(*clk);
    rst->next(0);
}

co_await delay(30);                   // wait 30 time units
co_await posedge(clk);                // wait for rising edge

// Multi-condition wait — returns winning trigger index
int w = co_await any(posedge(clk), delay(200));
// w == 0: posedge(clk) fired first
// w == 1: delay(200) fired first
```

For phase-aligned I/O, `sample` runs before DUT eval and `drive` runs after:

```cpp
sim.sample(posedge(clk), [&] { bfm.sample_inputs(); });
sim.drive(posedge(clk),  [&] { bfm.drive_outputs(); });
```

> **Edge visibility (DUT-driven signals).** The kernel observes signal transitions
> after each eval, so `co_await posedge(dut_output)` works for DUT-driven outputs.
> `sample` runs before eval and can only see TB-written edges; `drive` runs after
> eval, so `triggered()` on a DUT output is true there — a behavior change vs.
> earlier versions where DUT-driven signals never triggered.

## Examples

### async_fifo

Dual-clock asynchronous FIFO testbench demonstrating basic triggers, software signals, coroutine processes, and VCD dump.

```bash
cd examples/async_fifo/sim/tb
make
```

### axis_async_fifo

AXI4-Stream async FIFO testbench using BFM with `sample`/`drive` phase hooks. Frame-fifo mode only (aux FIFO stores per-frame side-channel signals; bad frames are detected and discarded).

```bash
cd examples/axis_async_fifo/sim/tb
make
```

## Dependencies

- C++20 (coroutines)
- Verilator 5.x
- Make

## License

MIT License — see [LICENSE](LICENSE).

---

<span id="cn">corosim</span>
===========================

**corosim** 是一个基于 C++20 协程的 [Verilator](https://www.veripool.org/verilator/) 协同仿真引擎。它将 `co_await` 映射为 delta 周期和定时事件调度，用于编写并发硬件测试台。

## 特性

- **协程并发** — `co_await posedge()`、`co_await delay()`、`co_await any()`
- **非阻塞信号** — `sig.next(val)`，delta 周期调度
- **触发进程** — `sim.always()` 重复触发，`sim.instance()` 一次性协程
- **相位 I/O** — `sim.sample()` / `sim.drive()` 用于 BFM 总线事务
- **软件信号** — 无需 DUT 绑定的跨进程同步

## 快速开始

### 1. 安装依赖
```bash
# macOS (使用 Homebrew)
brew install verilator

# Ubuntu/Debian
sudo apt-get install verilator g++ make
```

### 2. 克隆并运行
```bash
git clone https://github.com/dozecat/corosim.git
cd corosim/examples/async_fifo/sim/tb

make        # 编译并运行仿真
```

### 3. 查看波形
```bash
gtkwave waveform.vcd
```

## 项目结构

```
corosim/
├── src/                        # 库源码
│   ├── corosim.hpp             # 总头文件
│   ├── api/                    # 公共门面：Simulator<TOP>、Module、Dut
│   ├── core/                   # 引擎核心：Kernel、types、detail
│   ├── signal/                 # SignalVal、Signal<T>、wide、registry
│   ├── trigger/                # 触发规格 + delay/any 等待者
│   ├── coroutine/              # Task、Coroutine、WaitGroup、CoroutineManager
│   └── scheduler/              # delta 周期 + 定时事件调度
└── examples/
    ├── async_fifo/             # 双时钟异步 FIFO（基础）
    └── axis_async_fifo/        # AXI4-Stream 异步 FIFO（高级，带 BFM）
```

## API 参考

### Simulator&lt;TOP&gt; — 用户接口

大部分测试台通过 `Simulator<TOP>` 与引擎交互：

```cpp
#include "corosim.hpp"
using namespace corosim;

Vasync_fifo top;
Simulator sim(top);

auto& clk = sim.sig(top.clk);         // 绑定 DUT 信号
auto& rst = sim.sig<uint8_t>();       // 纯软件信号

sim.clock(clk, 10);                   // 每 5 时间单位翻转 clk

sim.run(500, [&](sim_time t) { tfp.dump(t); });
```

使用 `Dut<TOP>` 进行成员指针绑定：
```cpp
Dut<TOP> dut(top, sim.signals());
auto& clk = dut.sig(&TOP::clk);
auto& rst = dut.sig(&TOP::rst);
```

### Signal — Verilator 信号绑定

```cpp
Signal<uint8_t>  sig(&top->sig_field);  // 绑定 Verilator 信号
Signal<bool>     flag;                  // 纯软件信号

sig.next(val);    // 非阻塞赋值（delta 结束时提交）
sig.read();       // 读当前值
auto v = sig;     // 隐式读取

// Verilator 类型映射：
Signal<uint8_t>  rst(&top.rst);     // 1-bit   → CData
Signal<uint8_t>  data(&top.data);   // 8-bit   → CData
Signal<uint16_t> wide(&top.wide);   // 16-bit  → SData
```

宽向量（`VlWide<N>`）使用 `Signal<VlWide<N>>`。

### 触发事件

| 触发器 | 类型限制 | `sim.always()` | `co_await` |
|--------|----------|:---:|:---:|
| `posedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `negedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `change(sig)` | 任意 `Signal<T>` | ✓ | ✓ |
| `delay(n)` | — | ✓ | ✓ |

`posedge`/`negedge` 仅支持 1 字节信号；`change` 支持任意宽度。

### 进程

```cpp
// 触发重复执行
sim.always(posedge(clk), [&] { cnt.next(cnt.read() + 1); });
sim.always(delay(5), [&] { clk.next(!clk.read()); });

// 一次性协程 (lambda)
sim.instance([&]() -> Task {
    co_await delay(30);
    go.next(true);
});

// 一次性协程 (独立函数)
sim.instance(reset, &rst, &wr_clk);
```

协程函数中使用 `co_await` 同步：

```cpp
Task reset(Signal<uint8_t>* rst, Signal<uint8_t>* clk) {
    rst->next(1);
    for (int i = 0; i < 5; i++) co_await posedge(*clk);
    rst->next(0);
}

co_await delay(30);
co_await posedge(clk);

// 多条件等待 — 返回先触发的索引
int w = co_await any(posedge(clk), delay(200));
// w == 0: posedge(clk) 先到
// w == 1: delay(200) 先到
```

`sample` 在 DUT eval 前执行，`drive` 在 eval 后执行：
```cpp
sim.sample(posedge(clk), [&] { bfm.sample_inputs(); });
sim.drive(posedge(clk),  [&] { bfm.drive_outputs(); });
```

> **边沿可见性（DUT 驱动信号）。** 引擎在每次 eval 后统一观测信号跳变，
> 因此 `co_await posedge(DUT输出)` 对 DUT 驱动输出同样生效。
> `sample` 在 eval 前，只能看到 TB 写入产生的边沿；`drive` 在 eval 后，
> 此时 `triggered(DUT输出)` 为 true——这是相对旧版的行为变化（旧版 DUT 驱动信号永不触发）。

## 示例

### async_fifo

双时钟异步 FIFO 测试，演示基本触发、软件信号、协程进程和 VCD 波形。

```bash
cd examples/async_fifo/sim/tb
make
```

### axis_async_fifo

AXI4-Stream 异步 FIFO 测试，使用 BFM + `sample`/`drive` 相位钩子。仅 frame-fifo 模式（aux FIFO 存储帧级 side-channel 信号，检测并丢弃坏帧）。

```bash
cd examples/axis_async_fifo/sim/tb
make
```

## 依赖

- C++20（协程支持）
- Verilator 5.x
- Make

## 版权说明

MIT License — 见 [LICENSE](LICENSE)。
