#include <cstdio>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "corosim.hpp"
#include "Vasync_fifo.h"

using namespace corosim;

Proc reset_proc(Signal<bool>* rst, Signal<uint8_t>* wr_clk) {
    std::printf("[clock_cycles] reset hold for 3 clocks\n");
    rst->next(1);
    for (int i = 0; i < 3; i++) co_await posedge(*wr_clk);
    rst->next(0);
    std::printf("[clock_cycles] reset done\n");
}

Proc write_proc(Signal<bool>* rst, Signal<uint8_t>* wr_clk,
                Signal<bool>* wr_en, Signal<uint8_t>* wr_data,
                Signal<bool>* wr_full, Signal<bool>* go) {
    co_await posedge(*go);
    std::printf("[Event] write driver started\n");

    int wr_ok = 0;
    while (true) {
        co_await posedge(*wr_clk);
        if (rst->read()) continue;
        wr_en->next(1);
        wr_data->next((uint8_t)(wr_ok & 0xFF));
        if (!wr_full->read()) wr_ok++;
    }
}

int main(int argc, char* argv[]) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vasync_fifo top;
    VerilatedVcdC tfp;
    top.trace(&tfp, 99);
    tfp.open("waveform.vcd");

    Signal<bool>     rst(&top.rst);
    Signal<uint8_t>  wr_clk(&top.wr_clk);
    Signal<uint8_t>  wr_data(&top.wr_data);
    Signal<bool>     wr_en(&top.wr_en);
    Signal<bool>     wr_full(&top.wr_full);
    Signal<bool>     wr_overflow(&top.wr_overflow);
    Signal<uint8_t>  rd_clk(&top.rd_clk);
    Signal<bool>     rd_en(&top.rd_en);
    Signal<bool>     rd_empty(&top.rd_empty);

    Engine sim;

    always(delay(2),  [&] { wr_clk.next(!wr_clk.read()); });
    always(delay(10), [&] { rd_clk.next(!rd_clk.read()); });

    Signal<bool> go, overflow_evt, timeout_evt;

    proc([&]() -> Proc { return reset_proc(&rst, &wr_clk); });
    proc([&]() -> Proc {
        return write_proc(&rst, &wr_clk, &wr_en, &wr_data, &wr_full, &go);
    });

    always(posedge(rd_clk), [&] {
        rd_en.next(!rd_empty.read() ? 1 : 0);
    });

    int overflow_cnt = 0;
    always_comb([&] { if (wr_overflow.read()) overflow_cnt++; });

    // standalone function avoids Apple Clang coroutine capture bug
    static auto go_trigger = [](Signal<bool>* g) -> Proc {
        co_await delay(25);
        std::printf("[Event] go.set()\n");
        g->next(true);
    };
    proc([&]() -> Proc { return go_trigger(&go); });

    always(posedge(wr_clk), [&] {
        if (wr_overflow.read()) overflow_evt.next(true);
    });
    static auto timeout_trigger = [](Signal<bool>* t) -> Proc {
        co_await delay(200);
        t->next(true);
    };
    proc([&]() -> Proc { return timeout_trigger(&timeout_evt); });

    sim.init(&top, [&](sim_time t) { tfp.dump(t); });
    sim.run(500);
    tfp.close();

    std::printf("simulation done, overflow=%d\n", overflow_cnt);
    return 0;
}
