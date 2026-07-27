#include "scheduler.hpp"
#include <algorithm>

namespace corosim {

TimerId Scheduler::schedule_timer(sim_time deadline, std::coroutine_handle<> h) {
    TimerId tid{next_timer_id_++};
    timed_queue_.push({deadline, tid, h});
    return tid;
}

void Scheduler::schedule_monitor(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, WaitId wid) {
    if (monitor_processing_) {
        pending_monitor_.push_back({sig, edge, h, wid});
    } else {
        monitor_queue_.push_back({sig, edge, h, wid});
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
        if (!entry.wid.valid()) continue;
        if (!entry.handle || entry.handle.done()) continue;

        bool triggered = false;
        switch (entry.edge) {
        case TriggerInfo::POSEDGE:
            triggered = entry.sig->had_posedge();
            break;
        case TriggerInfo::NEGEDGE:
            triggered = entry.sig->had_negedge();
            break;
        case TriggerInfo::CHANGE:
            triggered = entry.sig->is_dirty();
            break;
        default:
            break;
        }
        if (triggered) {
            entry.wid.invalidate();
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
    while (!timed_queue_.empty() && timed_queue_.top().deadline <= now_) {
        auto entry = timed_queue_.top();
        timed_queue_.pop();
        if (!entry.handle || entry.handle.done()) continue;
        entry.handle.resume();
    }

    for (auto& h : tick_hooks_) if (h) h();
    sigs_.commit_all();

    for (auto& h : pre_eval_hooks_) if (h) h();

    if (eval_fn_) eval_fn_();

    for (auto& h : post_eval_hooks_) if (h) h();
    sigs_.commit_all();
    for (auto& h : commit_eval_hooks_) if (h) h();

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
        if (iteration >= max_delta_) break;
    }

    process_monitor_queue();

    for (auto& h : tick_end_hooks_) if (h) h();
    sigs_.clear_edge_flags();

    if (dump_fn_) dump_fn_(now_);
}

void Scheduler::run(sim_time duration) {
    run_one_tick();
    for (now_ = 1; now_ <= duration; now_++) {
        run_one_tick();
    }
}

} // namespace corosim
