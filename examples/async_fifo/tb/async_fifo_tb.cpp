/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        async_fifo_tb.cpp
 * @brief       Coroutine testbench for async_fifo with DEPTH=16
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Demonstrates all corosim features:
 *              - sim.always()   for clock gen, write/read drivers
 *              - sim.task()     for coroutine processes (reset)
 *              - sim.always_comb()  for combinational logic
 *              - clock_cycles()     wait for N clock edges
 *              - Event             manual synchronization
 *              - first()           wait for first of multiple events
 *              - pre_eval/post_eval (vaxivip BFM compat)
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/25  Initial release
 ******************************************************************************/

#include <cstdio>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "corosim.hpp"
#include "Vasync_fifo.h"

using namespace corosim;

Task reset_proc(Signal<bool>* rst, Signal<uint8_t>* wr_clk) {
    std::printf("[clock_cycles] reset hold for 3 clocks\n");
    rst->next(1);
    co_await clock_cycles(*wr_clk, 3);
    rst->next(0);
    std::printf("[clock_cycles] reset done\n");
}

Task write_proc(Signal<bool>* rst, Signal<uint8_t>* wr_clk,
                Signal<bool>* wr_en, Signal<uint8_t>* wr_data,
                Signal<bool>* wr_full, Event* go) {
    co_await go->wait();
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

    sim.always(delay(2),  [&] { wr_clk.next(!wr_clk.read()); });
    sim.always(delay(10), [&] { rd_clk.next(!rd_clk.read()); });

    Event go, overflow_evt, timeout_evt;

    sim.task([&]() -> Task { return reset_proc(&rst, &wr_clk); });
    sim.task([&]() -> Task {
        return write_proc(&rst, &wr_clk, &wr_en, &wr_data,
                          &wr_full, &go);
    });

    sim.always(posedge(rd_clk), [&] {
        rd_en.next(!rd_empty.read() ? 1 : 0);
    });

    int overflow_cnt = 0;
    sim.always_comb([&] { if (wr_overflow.read()) overflow_cnt++; });

    sim.task([&]() -> Task {
        co_await delay(25);
        std::printf("[Event] go.set()\n");
        go.set();
    });

    sim.always(posedge(wr_clk), [&] {
        if (wr_overflow.read()) overflow_evt.set();
    });
    sim.task([&]() -> Task {
        co_await delay(200);
        timeout_evt.set();
    });
    sim.task([&]() -> Task {
        int w;
        co_await first(w, overflow_evt, timeout_evt);
        std::printf("[first] winner=%d (0=overflow, 1=timeout)\n", w);
    });

    sim.init(&top, [&](sim_time t) { tfp.dump(t); });
    sim.run(500);
    tfp.close();

    std::printf("simulation done, overflow=%d\n", overflow_cnt);
    return 0;
}
