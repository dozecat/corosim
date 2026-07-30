/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        kernel.cpp
 * @brief       Kernel method implementations
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Wires ProcessManager cancel callbacks and forwards waits to the
 *              scheduler.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#include "kernel.hpp"
#include "process/process.hpp"

#include <verilated.h>
#include <cassert>

namespace corosim {

namespace {
/** @brief Resolve owning Process from a coroutine handle's promise. */
Process* get_process_from_handle(std::coroutine_handle<> h) {
    return Task::promise_type::from_handle(h).process;
}
}

Kernel::Kernel()
    : sched_(signals_) {
    process_manager_.set_kernel(this);
    // any(): when one wait fires, invalidate sibling WaitIds on the same process.
    sched_.set_cancel_fn([](auto h, auto wid) {
        auto* proc = get_process_from_handle(h);
        if (proc) proc->waits().cancel_others(wid);
    });
    // Signal deregistration notifies scheduler to clean monitor entries.
    signals_.on_signal_unregistered = [this](SignalBase* s) { sched_.on_signal_destroy(s); };
}

Kernel::~Kernel() {}

/** @brief Push simulation time into the Verilator context for VCD dumps. */
void Kernel::set_verilator_time(sim_time t) {
    if (top_) {
        auto* ctp = static_cast<VerilatedModel*>(top_)->contextp();
        if (ctp) ctp->time(t);
    }
}

void Kernel::register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx, int* fired) {
    auto* proc = get_process_from_handle(h);
    assert(proc && "coroutine handle not found in ProcessManager");
    auto* wid = proc->waits().add_edge_watch(sig, edge);
    sched_.schedule_monitor(sig, edge, h, wid, fire_idx, fired);
}

void Kernel::register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx, int* fired) {
    auto* proc = get_process_from_handle(h);
    assert(proc && "coroutine handle not found in ProcessManager");
    auto* wid = proc->waits().add_delay_watch(TimerId{0});
    auto deadline = sched_.now() + interval;
    TimerId tid = sched_.schedule_timer(deadline, h, wid, fire_idx, fired);
    wid->target = tid;
    (void)tid;
}

/** @brief Run the scheduler, then rethrow the first process exception if any. */
void Kernel::run(sim_time duration) {
    sched_.run(duration);

    auto ep = process_manager_.collect_exceptions();
    process_manager_.cleanup_finished();

    if (ep) std::rethrow_exception(ep);
}

} // namespace corosim
