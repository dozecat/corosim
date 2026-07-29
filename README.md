![Language](https://img.shields.io/badge/Language-C%2B%2B20-f34b7d.svg) ![Simulation](https://img.shields.io/badge/Simulation-Verilator-007ec6.svg) ![License](https://img.shields.io/badge/License-MIT-yellow.svg) ![Coroutines](https://img.shields.io/badge/Coroutines-enabled-8A2BE2.svg)

[English](#en) | [中文](#cn)

---

<span id="en">corosim</span>
===========================

**corosim** is a C++20 coroutine-based co-simulation engine for [Verilator](https://www.veripool.org/verilator/), inspired by [MyHDL](https://github.com/jandecaluwe/myhdl) and [cocotb](https://github.com/cocotb/cocotb). It lets you write concurrent hardware testbenches using `co_await` — no manual state machines, no manual eval-loop management.

## Features

| Feature | API | Description |
|---------|-----|-------------|
| **Non-blocking assignment** | `sig.next(val)` | Schedule a value — applied at end of delta cycle |
| **Clock-edge trigger** | `sim.always(posedge(clk), fn)` | Auto-repeat process on rising edge |
| **Delay trigger** | `sim.always(delay(n), fn)` | Fire every `n` time units |
| **Coroutine process** | `sim.instance(fn, args...)` | One-shot coroutine with `co_await` |
| **Multi-condition wait** | `co_await any(triggers...)` | Wait for first of multiple triggers |
| **Software signal** | `Signal<bool>()` | Default-constructed, no DUT binding |
| **Clock generator** | `sim.clock(sig, period)` | Toggle a signal every `period/2` |
| **sample / drive** | `sim.sample(...)` / `sim.drive(...)` | Phase-aligned I/O sampling |
| **sample / drive** | `sim.sample(...)` / `sim.drive(...)` | Phase-aligned I/O sampling |

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
│   ├── core/                   # Kernel, Sim<TOP>, Dut
│   ├── signal/                 # Signal<T>, wide, helpers
│   ├── trigger/                # Edge, delay, any waiters
│   ├── process/                # Task, Process, ProcessManager
│   └── scheduler/              # Delta-cycle + timed event engine
└── examples/
    ├── async_fifo/             # Dual-clock async FIFO (basic)
    └── axis_async_fifo/        # AXI4-Stream async FIFO (advanced, with BFM)
```

## API Reference

### Sim&lt;TOP&gt; — User-Facing Facade

Most testbenches interact through `Sim<TOP>`, which owns a `Kernel` and provides signal binding, process registration, and clock generation:

```cpp
#include "corosim.hpp"
using namespace corosim;

Vasync_fifo top;
Sim sim(top);                         // wrap the Verilator top

auto& clk = sim.sig(top.clk);         // bind a DUT signal
auto& rst = sim.sig<uint8_t>();       // software-only signal (no DUT)

sim.clock(clk, 10);                   // toggle clk every 5 time units

sim.run(500, [&](sim_time t) { tfp.dump(t); });
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

### Dut&lt;TOP&gt; — Pointer-to-Member Binding

```cpp
Dut<TOP> dut(top, sim.signals());
auto& clk = dut.sig(&TOP::clk);     // bind via pointer-to-member
auto& rst = dut.sig(&TOP::rst);
```

### Triggers

| Trigger | Type constraint | `sim.always()` | `co_await` |
|---------|----------------|:---:|:---:|
| `posedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `negedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `change(sig)` | any `Signal<T>` | ✓ | ✓ |
| `delay(n)` | — | ✓ | ✓ |

`posedge`/`negedge` require 1-byte signal types; `change` works with any signal width.

### Process Registration

```cpp
// Auto-repeat on every posedge
sim.always(posedge(clk), [&] {
    cnt.next(cnt.read() + 1);
});

// Auto-repeat every 5 time units
sim.always(delay(5), [&] { clk.next(!clk.read()); });

// Coroutine process (lambda)
sim.instance([&]() -> Task {
    co_await delay(30);
    go.next(true);
});

// Coroutine process (standalone function)
sim.instance(reset_proc, &rst, &wr_clk);
```

### Coroutine Primitives

```cpp
// Standalone coroutine function
Task reset_proc(Signal<uint8_t>* rst, Signal<uint8_t>* clk) {
    rst->next(1);
    for (int i = 0; i < 5; i++) co_await posedge(*clk);
    rst->next(0);
}

co_await delay(30);                   // wait 30 time units
co_await posedge(clk);                // wait for rising edge

// Software signal for cross-process sync
Signal<bool> flag;
// process A:
co_await posedge(flag);
// process B:
flag.next(true);

// Multi-condition wait — returns winning trigger index
int w = co_await any(posedge(clk), delay(200));
// w == 0: posedge(clk) fired first
// w == 1: delay(200) fired first
```

### Clock Generator

```cpp
sim.clock(sig, 10);        // toggle sig every 5 time units
```

This is equivalent to:
```cpp
sim.always(delay(5), [&] { sig.next(!sig.read()); });
```

### sample / drive — Phase-Aligned Access

`sample` runs before DUT eval (phase 2), `drive` runs after (phase 4):

```cpp
sim.sample(posedge(clk), [&] { bfm.sample_inputs(); });
sim.drive(posedge(clk),  [&] { bfm.drive_outputs(); });
```

## Examples

### async_fifo

Dual-clock asynchronous FIFO testbench demonstrating basic triggers, software signals, coroutine processes, and VCD dump.

```bash
cd examples/async_fifo/sim/tb
make
```

### axis_async_fifo

AXI4-Stream async FIFO testbench using BFM with `sample`/`drive` phase hooks. Four configurations via Verilator parameters `GFRAME_FIFO` and `GALWAYS_RECEIVE`.

```bash
cd examples/axis_async_fifo/sim/tb
make            # FRAME=0, ALWAYS_RECEIVE=0
make frame      # FRAME=1, ALWAYS_RECEIVE=0
make test-all   # all 4 configurations
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

**corosim** 是一个基于 C++20 协程的 [Verilator](https://www.veripool.org/verilator/) 协同仿真引擎，灵感来自 [MyHDL](https://github.com/jandecaluwe/myhdl) 和 [cocotb](https://github.com/cocotb/cocotb)。使用 `co_await` 编写并发硬件测试台，无需手动状态机，无需手动管理 eval 循环。

## 特性

| 功能 | API | 说明 |
|------|-----|------|
| 非阻塞赋值 | `sig.next(val)` | delta 周期结束时提交 |
| 时钟沿触发 | `sim.always(posedge(clk), fn)` | 上升沿自动重复 |
| 时间触发 | `sim.always(delay(n), fn)` | 每 n 时间单位触发 |
| 协程进程 | `sim.instance(fn, args...)` | 支持参数的协程，内部 `co_await` |
| 多条件等待 | `co_await any(triggers...)` | 最先到达者恢复 |
| 软件信号 | `Signal<bool>()` | 默认构造，不绑定 DUT |
| 时钟生成 | `sim.clock(sig, period)` | 每 period/2 翻转一次 |
| sample / drive | `sim.sample(...)` / `sim.drive(...)` | 相位对齐的 I/O 采样 |

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
│   ├── core/                   # 引擎核心、Sim<TOP>、Dut
│   ├── signal/                 # Signal<T>、wide、helpers
│   ├── trigger/                # 边沿、延时、多条件等待
│   ├── process/                # Task、Process、ProcessManager
│   └── scheduler/              # delta 周期 + 定时事件调度
└── examples/
    ├── async_fifo/             # 双时钟异步 FIFO（基础）
    └── axis_async_fifo/        # AXI4-Stream 异步 FIFO（高级，带 BFM）
```

## API 参考

### Sim&lt;TOP&gt; — 用户接口

大部分测试台通过 `Sim<TOP>` 与引擎交互：

```cpp
#include "corosim.hpp"
using namespace corosim;

Vasync_fifo top;
Sim sim(top);

auto& clk = sim.sig(top.clk);         // 绑定 DUT 信号
auto& rst = sim.sig<uint8_t>();       // 纯软件信号

sim.clock(clk, 10);                   // 每 5 时间单位翻转 clk

sim.run(500, [&](sim_time t) { tfp.dump(t); });
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

### Dut&lt;TOP&gt; — 成员指针绑定

```cpp
Dut<TOP> dut(top, sim.signals());
auto& clk = dut.sig(&TOP::clk);
auto& rst = dut.sig(&TOP::rst);
```

### 触发事件

| 触发器 | 类型限制 | `sim.always()` | `co_await` |
|--------|----------|:---:|:---:|
| `posedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `negedge(sig)` | `sizeof(T) <= 1` | ✓ | ✓ |
| `change(sig)` | 任意 `Signal<T>` | ✓ | ✓ |
| `delay(n)` | — | ✓ | ✓ |

`posedge`/`negedge` 仅支持 1 字节信号；`change` 支持任意宽度。

### 进程注册

```cpp
sim.always(posedge(clk), [&] { cnt.next(cnt.read() + 1); });
sim.always(delay(5), [&] { clk.next(!clk.read()); });

sim.instance([&]() -> Task {
    co_await delay(30);
    go.next(true);
});

sim.instance(reset_proc, &rst, &wr_clk);
```

### 协程原语

```cpp
Task reset_proc(Signal<uint8_t>* rst, Signal<uint8_t>* clk) {
    rst->next(1);
    for (int i = 0; i < 5; i++) co_await posedge(*clk);
    rst->next(0);
}

co_await delay(30);
co_await posedge(clk);

// 多条件等待
int w = co_await any(posedge(clk), delay(200));
// w == 0: posedge(clk) 先到
// w == 1: delay(200) 先到
```

### 时钟生成

```cpp
sim.clock(sig, 10);   // 每 5 时间单位翻转一次
```

等价于：
```cpp
sim.always(delay(5), [&] { sig.next(!sig.read()); });
```

### sample / drive

```cpp
sim.sample(posedge(clk), [&] { bfm.sample_inputs(); });
sim.drive(posedge(clk),  [&] { bfm.drive_outputs(); });
```

## 示例

### async_fifo

双时钟异步 FIFO 测试，演示基本触发、软件信号、协程进程和 VCD 波形。

```bash
cd examples/async_fifo/sim/tb
make
```

### axis_async_fifo

AXI4-Stream 异步 FIFO 测试，使用 BFM + `sample`/`drive` 相位钩子。通过 Verilator 参数 `GFRAME_FIFO` 和 `GALWAYS_RECEIVE` 支持 4 种配置。

```bash
cd examples/axis_async_fifo/sim/tb
make            # FRAME=0, ALWAYS_RECEIVE=0
make frame      # FRAME=1, ALWAYS_RECEIVE=0
make test-all   # 全部 4 种配置
```

## 依赖

- C++20（协程支持）
- Verilator 5.x
- Make

## 版权说明

MIT License — 见 [LICENSE](LICENSE)。
