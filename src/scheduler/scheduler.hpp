/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        scheduler.hpp
 * @brief       Delta-cycle and timed event scheduler
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Advances simulation time, runs eval phases, and monitor resumes.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <functional>
#include <queue>
#include <vector>
#include "core/types.hpp"
#include "signal/signal_base.hpp"
#include "signal/signal_registry.hpp"

namespace corosim {

/**
 * @brief Owns timed queue, edge monitors, and one-tick phase sequencing.
 *
 * Tick order: timed resume → commit → pre_eval → eval → post_eval → commit →
 * monitor resume → commit → clear edges → dump.
 */
class Scheduler {
public:
    explicit Scheduler(SignalRegistry& signals) : signals_(signals) {}

    void set_eval_fn(std::function<void()> fn) { eval_fn_ = std::move(fn); }
    void set_dump_fn(std::function<void(sim_time)> fn) { dump_fn_ = std::move(fn); }
    void set_cancel_fn(std::function<void(std::coroutine_handle<>, WaitId*)> fn) { cancel_others_fn_ = std::move(fn); }

    sim_time now() const { return now_; }

    template <typename Fn>
    void on_pre_eval(Fn&& fn) { pre_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }
    template <typename Fn>
    void on_post_eval(Fn&& fn) { post_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }

    bool had_edge(SignalBase* sig, TriggerType edge) const {
        switch (edge) {
        case TriggerInfo::POSEDGE: return sig->had_posedge();
        case TriggerInfo::NEGEDGE: return sig->had_negedge();
        case TriggerInfo::CHANGE:  return sig->has_changed();
        default: return false;
        }
    }

    /** @brief Schedule coroutine resume at absolute @p deadline. */
    TimerId schedule_timer(sim_time deadline, std::coroutine_handle<> h, WaitId* wid, int fire_idx = -1, int* fired = nullptr);
    /** @brief Watch @p sig for @p edge and resume @p h when it fires. */
    void schedule_monitor(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, WaitId* wid, int fire_idx = -1, int* fired = nullptr);

    /** @brief Advance time until @p duration or no valid timers remain. */
    void run(sim_time duration);

private:
    void run_one_tick();
    void process_monitor_queue();

    SignalRegistry& signals_;

    std::function<void(std::coroutine_handle<>, WaitId*)> cancel_others_fn_;

    struct TimedEntry {
        sim_time deadline;
        TimerId timer_id;
        std::coroutine_handle<> handle;
        WaitId* wid = nullptr;
        int* fire_idx = nullptr;
        int fire_value = -1;
        bool operator>(const TimedEntry& o) const { return deadline > o.deadline; }
    };
    std::priority_queue<TimedEntry, std::vector<TimedEntry>, std::greater<>> timed_queue_;
    uint64_t next_timer_id_ = 1;

    struct MonitorEntry {
        SignalBase* sig;
        TriggerType edge;
        std::coroutine_handle<> handle;
        WaitId* wid = nullptr;
        int* fire_idx = nullptr;
        int fire_value = -1;
    };
    std::vector<MonitorEntry> monitor_queue_;
    std::vector<MonitorEntry> pending_monitor_;
    bool monitor_processing_ = false;

    std::vector<std::function<void()>> pre_eval_hooks_;
    std::vector<std::function<void()>> post_eval_hooks_;

    std::function<void()> eval_fn_;
    std::function<void(sim_time)> dump_fn_;

    sim_time now_ = 0;
};

} // namespace corosim
