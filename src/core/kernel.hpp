/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        kernel.hpp
 * @brief       Simulation kernel composing signals, scheduler, and processes
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Public API for always/instance/sample/drive and awaiter wait
 *              registration.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <functional>
#include <memory>
#include <cassert>

#include "core/types.hpp"
#include "signal/signal_registry.hpp"
#include "scheduler/scheduler.hpp"
#include "process/process_manager.hpp"
#include "process/task.hpp"
#include "trigger/edge.hpp"
#include "trigger/delay.hpp"

namespace corosim {

/**
 * @brief Core simulation engine.
 *
 * Owns SignalRegistry, Scheduler, and ProcessManager. Template methods are
 * defined in kernel_impl.hpp.
 */
class Kernel {
public:
    Kernel();
    ~Kernel();

    SignalRegistry& signals() { return signals_; }
    Scheduler& scheduler() { return sched_; }
    ProcessManager& process_manager() { return process_manager_; }

    void set_top(void* top) { top_ = top; }
    void set_verilator_time(sim_time t);

    sim_time now() const { return sched_.now(); }

    bool had_edge(SignalBase* sig, TriggerType edge) const { return sched_.had_edge(sig, edge); }

    /** @brief Spawn a repeating process on @p t (edge or delay). */
    template <typename Trigger, typename Fn>
    void always(Trigger t, Fn fn);

    /** @brief Spawn a one-shot coroutine process. */
    template <typename Fn, typename... Args>
    Process* instance(Fn&& fn, Args&&... args);

    /** @brief Run @p fn on trigger before DUT eval (sample phase). */
    template <typename Trigger, typename Fn>
    void sample(Trigger t, Fn fn);

    /** @brief Run @p fn on trigger after DUT eval (drive phase). */
    template <typename Trigger, typename Fn>
    void drive(Trigger t, Fn fn);

    /** @brief Hook invoked before each DUT eval. */
    template <typename Fn>
    void pre_eval(Fn&& fn);

    /** @brief Hook invoked after each DUT eval. */
    template <typename Fn>
    void post_eval(Fn&& fn);

    /** @brief Run until @p duration or no pending timers. */
    void run(sim_time duration);

    /** @name Awaiter registration (internal) */
    ///@{
    void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx = -1, int* fired = nullptr);
    void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx = -1, int* fired = nullptr);
    ///@}

private:
    SignalRegistry signals_;
    Scheduler sched_;
    ProcessManager process_manager_;

    void* top_ = nullptr;

    template <typename Fn>
    Process* add_process(Fn&& fn) { return process_manager_.spawn(std::forward<Fn>(fn)); }
};

} // namespace corosim
