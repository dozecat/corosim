#include <cstdio>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "corosim.hpp"
#include "Vasync_fifo.h"

using namespace corosim;

static Task reset_proc(Signal<bool>* rst, Signal<uint8_t>* wr_clk) {
    std::printf("[clock_cycles] reset hold for 3 clocks\n");
    rst->next(1);
    for (int i = 0; i < 3; i++) co_await posedge(*wr_clk);
    rst->next(0);
    std::printf("[clock_cycles] reset done\n");
}

static Task write_proc(Signal<bool>* rst, Signal<uint8_t>* wr_clk,
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

static Task go_trigger(Signal<bool>* g) {
    co_await delay(25);
    std::printf("[Event] go.set()\n");
    g->next(true);
}

static Task timeout_trigger(Signal<bool>* t) {
    co_await delay(200);
    t->next(true);
}

int main(int argc, char* argv[]) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vasync_fifo top;
    VerilatedVcdC tfp;
    top.trace(&tfp, 99);
    tfp.open("waveform.vcd");

    Kernel sim;

    Signal<bool>     rst(sim.signals(), &top.rst);
    Signal<uint8_t>  wr_clk(sim.signals(), &top.wr_clk);
    Signal<uint8_t>  wr_data(sim.signals(), &top.wr_data);
    Signal<bool>     wr_en(sim.signals(), &top.wr_en);
    Signal<bool>     wr_full(sim.signals(), &top.wr_full);
    Signal<bool>     wr_overflow(sim.signals(), &top.wr_overflow);
    Signal<uint8_t>  rd_clk(sim.signals(), &top.rd_clk);
    Signal<bool>     rd_en(sim.signals(), &top.rd_en);
    Signal<bool>     rd_empty(sim.signals(), &top.rd_empty);

    sim.always(delay(2),  [&] { wr_clk.next(!wr_clk.read()); });
    sim.always(delay(10), [&] { rd_clk.next(!rd_clk.read()); });

    Signal<bool> go(sim.signals()), overflow_evt(sim.signals()), timeout_evt(sim.signals());

    sim.instance(reset_proc, &rst, &wr_clk);
    sim.instance(write_proc, &rst, &wr_clk, &wr_en, &wr_data, &wr_full, &go);

    sim.always(posedge(rd_clk), [&] {
        rd_en.next(!rd_empty.read() ? 1 : 0);
    });

    int overflow_cnt = 0;
    sim.always_comb([&] { if (wr_overflow.read()) overflow_cnt++; });

    sim.instance(go_trigger, &go);

    sim.always(posedge(wr_clk), [&] {
        if (wr_overflow.read()) overflow_evt.next(true);
    });

    sim.instance(timeout_trigger, &timeout_evt);

    sim.init(&top, [&](sim_time t) { tfp.dump(t); });
    sim.run(500);
    tfp.close();

    std::printf("simulation done, overflow=%d\n", overflow_cnt);
    return 0;
}
