#include "scheduler.hpp"
#include <algorithm>
#include <stdexcept>

namespace corosim {

TimerId Scheduler::schedule_timer(sim_time deadline, std::coroutine_handle<> h, WaitId* wid, int fire_idx, int* fired) {
    TimerId tid{next_timer_id_++};
    timed_queue_.push({deadline, tid, h, wid, fired, fire_idx});
    return tid;
}

void Scheduler::schedule_monitor(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, WaitId* wid, int fire_idx, int* fired) {
    if (monitor_processing_) {
        pending_monitor_.push_back({sig, edge, h, wid, fired, fire_idx});
    } else {
        monitor_queue_.push_back({sig, edge, h, wid, fired, fire_idx});
    }
}

void Scheduler::schedule_delta(std::function<void()> callback) {
    delta_callbacks_.push_back(std::move(callback));
}

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

void Scheduler::run_one_tick() {
    // ── Write: timed resume + commit + pre_eval + eval ──
    while (!timed_queue_.empty() && timed_queue_.top().deadline <= now_) {
        auto entry = timed_queue_.top();
        timed_queue_.pop();
        if (!entry.handle || entry.handle.done()) continue;
        if (!entry.wid || !entry.wid->valid()) continue;
        if (entry.fire_idx) *entry.fire_idx = entry.fire_value;
        if (cancel_others_fn_) cancel_others_fn_(entry.handle, entry.wid);
        entry.handle.resume();
    }

    sigs_.commit_all();
    for (auto& h : pre_eval_hooks_) if (h) h();
    if (eval_fn_) eval_fn_();

    // ── Read: post_eval + commit + delta + monitor ──
    for (auto& h : post_eval_hooks_) if (h) h();
    sigs_.commit_all();

    for (size_t iteration = 0; ; iteration++) {
        bool dirty = false;
        for (auto& cb : delta_callbacks_) {
            if (cb) cb();
        }
        sigs_.commit_all();
        for (auto* sig : sigs_.dirty_signals()) {
            if (sig->is_dirty()) {
                dirty = true;
                sig->clear_dirty();
            }
        }
        if (!dirty) break;
        if (iteration >= max_delta_)
            throw std::runtime_error("Delta oscillation: max iterations exceeded");
    }

    process_monitor_queue();

    // ── Flip + Next ──
    sigs_.clear_edge_flags();
    if (dump_fn_) dump_fn_(now_);
}

void Scheduler::run(sim_time duration) {
    run_one_tick();
    now_ = 1;
    while (now_ <= duration) {
        while (!timed_queue_.empty() && timed_queue_.top().wid && !timed_queue_.top().wid->valid()) {
            timed_queue_.pop();
        }
        if (!timed_queue_.empty()) {
            sim_time next_deadline = timed_queue_.top().deadline;
            if (next_deadline > now_ && next_deadline <= duration) {
                now_ = next_deadline;
            }
        }
        if (now_ > duration) break;
        run_one_tick();
        now_++;
    }
}

} // namespace corosim
