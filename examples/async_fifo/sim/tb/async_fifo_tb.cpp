#include <cstdio>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "corosim.hpp"
#include "Vasync_fifo.h"

using namespace corosim;

int main(int argc, char* argv[]) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);

    Vasync_fifo top;
    VerilatedVcdC tfp;
    top.trace(&tfp, 99);
    tfp.open("waveform.vcd");

    Simulator sim(top);

    auto& rst    = sim.sig(top.rst);
    auto& wr_clk = sim.sig(top.wr_clk);
    auto& wr_data = sim.sig(top.wr_data);
    auto& wr_en  = sim.sig(top.wr_en);
    auto& wr_full = sim.sig(top.wr_full);
    auto& wr_overflow = sim.sig(top.wr_overflow);
    auto& rd_clk = sim.sig(top.rd_clk);
    auto& rd_en  = sim.sig(top.rd_en);
    auto& rd_empty = sim.sig(top.rd_empty);

    auto& go          = sim.sig<uint8_t>();
    auto& overflow_evt = sim.sig<uint8_t>();
    auto& timeout_evt = sim.sig<uint8_t>();

    sim.clock(wr_clk, 4);
    sim.clock(rd_clk, 20);

    sim.instance([&]() -> Task {
        std::printf("[clock_cycles] reset hold for 3 clocks\n");
        rst.next(1);
        for (int i = 0; i < 3; i++) co_await posedge(wr_clk);
        rst.next(0);
        std::printf("[clock_cycles] reset done\n");
    });

    sim.instance([&]() -> Task {
        co_await posedge(go);
        std::printf("[Event] write driver started\n");

        int wr_ok = 0;
        while (true) {
            co_await posedge(wr_clk);
            if (rst.read()) continue;
            wr_en.next(1);
            wr_data.next((uint8_t)(wr_ok & 0xFF));
            if (!wr_full.read()) wr_ok++;
        }
    });

    sim.always(posedge(rd_clk), [&] {
        rd_en.next(!rd_empty.read() ? 1 : 0);
    });

    sim.instance([&]() -> Task {
        co_await delay(25);
        std::printf("[Event] go.set()\n");
        go.next(true);
    });

    sim.always(posedge(wr_clk), [&] {
        if (wr_overflow.read()) overflow_evt.next(true);
    });

    // DUT-driven edge regression: wr_full is driven by the DUT, never by the TB.
    int full_edges = 0;
    sim.always(posedge(wr_full), [&] { full_edges++; });

    sim.instance([&]() -> Task {
        co_await delay(200);
        timeout_evt.next(true);
    });

    sim.run(500, [&](sim_time t) { tfp.dump(t); });
    tfp.close();

    std::printf("[check] DUT-driven wr_full posedges: %d\n", full_edges);
    COROSIM_CHECK(full_edges > 0);

    std::printf("simulation done\n");
    return 0;
}
