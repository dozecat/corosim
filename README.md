![Language](https://img.shields.io/badge/Language-C%2B%2B20-f34b7d.svg) ![Simulation](https://img.shields.io/badge/Simulation-Verilator-007ec6.svg) ![License](https://img.shields.io/badge/License-MIT-yellow.svg) ![Coroutines](https://img.shields.io/badge/Coroutines-C%2B%2B20-8A2BE2.svg)

[English](#en) | [中文](#cn)

---

<span id="en">corosim</span>
===========================

**corosim** is a C++20 coroutine-based co-simulation engine for [Verilator](https://www.veripool.org/verilator/), inspired by [MyHDL](https://github.com/jandecaluwe/myhdl) and [cocotb](https://github.com/cocotb/cocotb). It lets you write concurrent hardware testbenches using `co_await` — no manual state machines, no manual eval-loop management.

## ✨ Features

| Feature | API | Description |
|---------|-----|-------------|
| **Non-blocking assignment** | `sig.next(val)` | Schedule a value — applied at end of delta cycle |
| **Clock-edge trigger** | `sim.always(posedge(clk), fn)` | Auto-repeat process on rising edge |
| **Combinational trigger** | `sim.always_comb(fn)` | Fire on any input change (delta iteration) |
| **Delay trigger** | `sim.always(delay(n), fn)` | Fire every `n` time units |
| **Coroutine process** | `sim.proc(fn, args...)` | Coroutine process with internal `co_await` |
| **Multi-condition wait** | `co_await any(triggers...)` | Wait for the first of multiple triggers (edge, delay, signal) |
| **Software signal** | `Signal<bool>()` | Default-constructed signal for cross-process sync |
| **BFM compat** | `sim.pre_eval(fn)` / `sim.post_eval(fn)` | Hook into eval cycle (vaxivip BFM) |

## 🚀 Quick Start

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
cd corosim/examples/async_fifo/tb

make        # Compile and run simulation
```

### 3. View Waveform
```bash
gtkwave waveform.vcd
```

## 📖 API Reference

### Signal
```cpp
Signal<uint8_t> sig(&top->sig_field);  // bind to Verilator signal

Signal<bool> flag;                      // software-only signal (no Verilator binding)

sig.next(val);   // non-blocking assignment
sig.read();      // read current value
auto v = sig;    // implicit read
```

All Verilator signals are bound by pointer:
```cpp
Signal<uint8_t> rst(&top.rst);     // 1-bit → CData
Signal<uint8_t> data(&top.data);   // 8-bit → CData
Signal<uint16_t> wide(&top.wide);  // 16-bit → SData
```

### Triggers

| Trigger | Type constraint | Used in |
|---------|----------------|---------|
| `posedge(sig)` | `sizeof(T) <= 1` | `sim.always(...)`, `co_await` |
| `negedge(sig)` | `sizeof(T) <= 1` | `sim.always(...)`, `co_await` |
| `change(sig)` | any `Signal<T>` | `sim.always(...)`, `co_await` |
| `delay(n)` | — | `sim.always(...)`, `co_await` |

### Process Registration

```cpp
// Auto-repeat on every posedge (simple lambda)
sim.always(posedge(clk), [&] {
    cnt.next(cnt.read() + 1);
});

// Auto-repeat every 5 time units
sim.always(delay(5), [&] { clk.next(!clk.read()); });

// Combinational (delta-cycle iteration)
sim.always_comb([&] {
    y.next(a.read() & b.read());
});

// Coroutine process
sim.proc(reset_proc, &rst, &wr_clk);
```

### Coroutine Primitives

```cpp
// Standalone coroutine function
Task reset_proc(Signal<uint8_t>* rst, Signal<uint8_t>* clk) {
    rst->next(1);
    for (int i = 0; i < 5; i++) co_await posedge(*clk);   // wait 5 posedges
    rst->next(0);
}

// Inside another coroutine:
co_await delay(30);                   // wait 30 time units
co_await posedge(clk);                // wait for rising edge

// Software signal for cross-process sync
Signal<bool> flag;
// ... in process A:
co_await posedge(flag);               // wait for flag
// ... in process B:
flag.next(true);                      // notify

// Multi-condition wait (any of triggers)
int w = co_await any(posedge(clk), delay(200));
// w == 0: posedge(clk) fired first
// w == 1: delay(200) fired first
```

### vaxivip BFM Compatibility
```cpp
sim.pre_eval([&]  { bfm.update_input(); });
sim.post_eval([&] { bfm.update_output(); });
```

### sample_cb / drive_cb

```cpp
// Phase 2 (before eval): sample DUT outputs
sim.sample_cb(posedge(clk), [&] { bfm.sample_inputs(); });

// Phase 4 (after eval): drive DUT inputs
sim.drive_cb(posedge(clk), [&] { bfm.drive_outputs(); });
```

## 📁 Project Structure

```
corosim/
├── src/                        # Library source
│   ├── corosim.hpp             # Umbrella header
│   ├── task.hpp                # Task coroutine type
│   ├── delay.hpp               # Delay awaiter / trigger
│   ├── signal.hpp              # Signal<T> + edge/change triggers
│   ├── engine.hpp              # Engine declaration
│   ├── engine.cpp              # Engine: eval loop + scheduler
│   └── triggers.hpp            # any(): multi-trigger wait
└── examples/
    └── async_fifo/             # Dual-clock async FIFO
        ├── rtl/                #   RTL source
        │   └── async_fifo.v
        └── tb/                 #   Testbench
            ├── Makefile
            └── async_fifo_tb.cpp
```

## 🔧 Dependencies

- C++20 (coroutines)
- Verilator 5.x
- Make

## 📝 Acknowledgements

Inspired by [MyHDL](https://github.com/jandecaluwe/myhdl), [cocotb](https://github.com/cocotb/cocotb), and [vaxivip](https://github.com/dozecat/vaxivip).

## 📄 License

MIT License — see [LICENSE](LICENSE).

---

<span id="cn">corosim</span>
===========================

**corosim** 是一个基于 C++20 协程的 [Verilator](https://www.veripool.org/verilator/) 协同仿真引擎，灵感来自 [MyHDL](https://github.com/jandecaluwe/myhdl) 和 [cocotb](https://github.com/cocotb/cocotb)。使用 `co_await` 编写并发硬件测试台，无需手动状态机，无需手动管理 eval 循环。

## ✨ 特性

| 功能 | API | 说明 |
|------|-----|------|
| 非阻塞赋值 | `sig.next(val)` | 在 delta 周期结束时提交 |
| 时钟沿触发 | `sim.always(posedge(clk), fn)` | 上升沿自动重复 |
| 组合逻辑 | `sim.always_comb(fn)` | 输入变化自动触发（delta 迭代） |
| 时间触发 | `sim.always(delay(n), fn)` | 每 n 个时间单位触发一次 |
| 协程进程 | `sim.proc(fn, args...)` | 支持参数的协程进程，内部用 co_await 自调度 |
| 多条件等待 | `co_await any(triggers...)` | 多个触发条件中最先到者恢复 |
| 软件信号 | `Signal<bool>()` | 默认构造信号，用于跨进程同步 |
| BFM 兼容 | `sim.pre_eval(fn)` / `sim.post_eval(fn)` | 接入 eval 生命周期（vaxivip BFM） |

## 🚀 快速开始

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
cd corosim/examples/async_fifo/tb

make        # 编译并运行仿真
```

### 3. 查看波形
```bash
gtkwave waveform.vcd
```

## 📖 API 参考

### 信号
```cpp
Signal<uint8_t> sig(&top->sig_field);  // 绑定 Verilator 信号

Signal<bool> flag;                      // 纯软件信号（不绑定 Verilator）

sig.next(val);   // 非阻塞赋值
sig.read();      // 读当前值
auto v = sig;    // 隐式读取
```

所有信号通过指针绑定到 Verilator 信号字段：
```cpp
Signal<uint8_t> rst(&top.rst);     // 1-bit → CData
Signal<uint8_t> data(&top.data);   // 8-bit → CData
Signal<uint16_t> wide(&top.wide);  // 16-bit → SData
```

### 触发事件

| 触发器 | 类型限制 | 用途 |
|--------|----------|------|
| `posedge(sig)` | 仅 1 字节信号 | `sim.always(...)`, `co_await` |
| `negedge(sig)` | 仅 1 字节信号 | `sim.always(...)`, `co_await` |
| `change(sig)` | 任意 `Signal<T>` | `sim.always(...)`, `co_await` |
| `delay(n)` | — | `sim.always(...)`, `co_await` |

### 流程注册

```cpp
// 每个上升沿自动触发（简单 lambda）
sim.always(posedge(clk), [&] {
    cnt.next(cnt.read() + 1);
});

// 每 5 个时间单位触发一次
sim.always(delay(5), [&] { clk.next(!clk.read()); });

// 组合逻辑（delta 周期迭代）
sim.always_comb([&] {
    y.next(a.read() & b.read());
});

// 协程进程
sim.proc(reset_proc, &rst, &wr_clk);
```

### 协程原语

```cpp
// 独立协程函数
Task reset_proc(Signal<uint8_t>* rst, Signal<uint8_t>* clk) {
    rst->next(1);
    for (int i = 0; i < 5; i++) co_await posedge(*clk);   // 等 5 个上升沿
    rst->next(0);
}

// 在另一个协程内部：
co_await delay(30);                   // 等 30 时间单位
co_await posedge(clk);                // 等下个上升沿

// 软件信号跨进程同步
Signal<bool> flag;
// ... 进程 A:
co_await posedge(flag);               // 等信号
// ... 进程 B:
flag.next(true);                      // 通知

// 多条件等待
int w = co_await any(posedge(clk), delay(200));
// w == 0: posedge(clk) 先到
// w == 1: delay(200) 先到
```

### vaxivip BFM 兼容
```cpp
sim.pre_eval([&]  { bfm.update_input(); });
sim.post_eval([&] { bfm.update_output(); });
```

### sample_cb / drive_cb

```cpp
// Phase 2 (eval 前): 采样 DUT 输出
sim.sample_cb(posedge(clk), [&] { bfm.sample_inputs(); });

// Phase 4 (eval 后): 驱动 DUT 输入
sim.drive_cb(posedge(clk), [&] { bfm.drive_outputs(); });
```

## 📁 项目结构

```
corosim/
├── src/                        # 库源码
│   ├── corosim.hpp             # 总头文件
│   ├── task.hpp                # Task 协程类型
│   ├── delay.hpp               # Delay 等待体 / 触发器
│   ├── signal.hpp              # Signal<T> + 边沿/变化触发器
│   ├── engine.hpp              # Engine 声明
│   ├── engine.cpp              # Engine: eval 循环 + 调度器
│   └── triggers.hpp            # any(): 多触发条件等待
└── examples/
    └── async_fifo/             # 双时钟域异步 FIFO
        ├── rtl/                #   RTL 代码
        │   └── async_fifo.v
        └── tb/                 #   测试台
            ├── Makefile
            └── async_fifo_tb.cpp
```

## 🔧 依赖

- C++20（协程支持）
- Verilator 5.x
- Make

## 📝 致谢

灵感来自 [MyHDL](https://github.com/jandecaluwe/myhdl)、[cocotb](https://github.com/cocotb/cocotb) 和 [vaxivip](https://github.com/dozecat/vaxivip)。

## 📄 版权说明

MIT License — 见 [LICENSE](LICENSE)。
