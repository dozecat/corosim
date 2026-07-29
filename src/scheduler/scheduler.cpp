/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        scheduler.cpp
 * @brief       Scheduler method implementations
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Implements run_one_tick phases and timer/monitor queue
 *              processing.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#include "scheduler.hpp"
#include <algorithm>

namespace corosim {

TimerId Scheduler::schedule_timer(sim_time deadline, std::coroutine_handle<> h, WaitId* wid, int fire_idx, int* fired) {
    TimerId tid{next_timer_id_++};
    timed_queue_.push({deadline, tid, h, wid, fired, fire_idx});
    return tid;
}

void Scheduler::schedule_monitor(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, WaitId* wid, int fire_idx, int* fired) {
    // Defer inserts while process_monitor_queue() is iterating.
    if (monitor_processing_) {
        pending_monitor_.push_back({sig, edge, h, wid, fired, fire_idx});
    } else {
        monitor_queue_.push_back({sig, edge, h, wid, fired, fire_idx});
    }
}

/** @brief Resume monitors whose edge fired; keep the rest. */
void Scheduler::process_monitor_queue() {
    if (monitor_queue_.empty()) return;

    monitor_processing_ = true;
    decltype(monitor_queue_) kept;
    kept.reserve(monitor_queue_.size());

    for (auto& entry : monitor_queue_) {
        if (!entry.handle || entry.handle.done()) continue;
        if (!entry.wid || !entry.wid->valid()) continue;

        bool triggered = false;
        switch (entry.edge) {
        case TriggerInfo::POSEDGE:
            triggered = entry.sig->had_posedge();
            break;
        case TriggerInfo::NEGEDGE:
            triggered = entry.sig->had_negedge();
            break;
        case TriggerInfo::CHANGE:
            triggered = entry.sig->has_changed();
            break;
        default:
            break;
        }
        if (triggered) {
            if (entry.fire_idx) *entry.fire_idx = entry.fire_value;
            if (cancel_others_fn_) cancel_others_fn_(entry.handle, entry.wid);
            entry.wid->invalidate();
            entry.handle.resume();
        } else {
            kept.push_back(entry);
        }
    }
    monitor_queue_ = std::move(kept);
    monitor_processing_ = false;
    monitor_queue_.insert(monitor_queue_.end(), pending_monitor_.begin(), pending_monitor_.end());
    pending_monitor_.clear();
}

/**
 * @brief Execute one simulation tick at @c now_.
 *
 * Order: timed resume → commit → pre_eval → eval → post_eval → commit →
 * monitors → commit (NBA from edge processes) → clear edges → dump.
 */
void Scheduler::run_one_tick() {
    while (!timed_queue_.empty() && timed_queue_.top().deadline <= now_) {
        auto entry = timed_queue_.top();
        timed_queue_.pop();
        if (!entry.handle || entry.handle.done()) continue;
        if (!entry.wid || !entry.wid->valid()) continue;
        if (entry.fire_idx) *entry.fire_idx = entry.fire_value;
        if (cancel_others_fn_) cancel_others_fn_(entry.handle, entry.wid);
        entry.handle.resume();
    }

    signals_.commit_all();
    for (auto& h : pre_eval_hooks_) if (h) h();
    if (eval_fn_) eval_fn_();

    for (auto& h : post_eval_hooks_) if (h) h();
    signals_.commit_all();

    process_monitor_queue();
    // Edge processes resume after eval; commit their NBA in this same time
    // so wr_en/rd_en align with the clock edge in the waveform (HDL-like).
    signals_.commit_all();

    signals_.clear_edge_flags();
    if (dump_fn_) dump_fn_(now_);
}

/** @brief Drain timers up to @p duration, advancing @c now_ as needed. */
void Scheduler::run(sim_time duration) {
    run_one_tick();

    while (true) {
        while (!timed_queue_.empty()) {
            auto& top = timed_queue_.top();
            if (!top.wid || !top.wid->valid() || top.handle.done())
                timed_queue_.pop();
            else
                break;
        }
        if (timed_queue_.empty()) break;

        sim_time next_deadline = timed_queue_.top().deadline;
        if (next_deadline > duration) break;

        if (next_deadline > now_) now_ = next_deadline;
        run_one_tick();
    }
}

} // namespace corosim
