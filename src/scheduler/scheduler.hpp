#pragma once

#include <coroutine>
#include <functional>
#include <queue>
#include <vector>
#include "../core/types.hpp"
#include "../signal/signal_base.hpp"
#include "../signal/signal_registry.hpp"

namespace corosim {

class Scheduler {
public:
    explicit Scheduler(SignalRegistry& sigs) : sigs_(sigs) {}

    void set_eval_fn(std::function<void()> fn) { eval_fn_ = std::move(fn); }
    void set_dump_fn(std::function<void(sim_time)> fn) { dump_fn_ = std::move(fn); }
    void set_max_delta_iterations(size_t n) { max_delta_ = n; }

    sim_time now() const { return now_; }

    template <typename Fn>
    void on_tick(Fn&& fn) { tick_hooks_.emplace_back(std::forward<Fn>(fn)); }

    template <typename Fn>
    void on_pre_eval(Fn&& fn) { pre_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }

    template <typename Fn>
    void on_post_eval(Fn&& fn) { post_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }

    template <typename Fn>
    void on_commit_eval(Fn&& fn) { commit_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }

    template <typename Fn>
    void on_comb(Fn&& fn) { delta_callbacks_.emplace_back(std::forward<Fn>(fn)); }

    template <typename Fn>
    void on_tick_end(Fn&& fn) { tick_end_hooks_.emplace_back(std::forward<Fn>(fn)); }

    bool had_edge(SignalBase* sig, TriggerType edge) const {
        switch (edge) {
        case TriggerInfo::POSEDGE: return sig->had_posedge();
        case TriggerInfo::NEGEDGE: return sig->had_negedge();
        case TriggerInfo::CHANGE:  return sig->is_dirty();
        default: return false;
        }
    }

    TimerId schedule_timer(sim_time deadline, std::coroutine_handle<> h);
    void schedule_monitor(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, WaitId wid);
    void schedule_delta(std::function<void()> callback);

    void run(sim_time duration);

private:
    void run_one_tick();
    void process_monitor_queue();

    SignalRegistry& sigs_;

    struct TimedEntry {
        sim_time deadline;
        TimerId timer_id;
        std::coroutine_handle<> handle;
        bool operator>(const TimedEntry& o) const { return deadline > o.deadline; }
    };
    std::priority_queue<TimedEntry, std::vector<TimedEntry>, std::greater<>> timed_queue_;
    uint64_t next_timer_id_ = 1;

    struct MonitorEntry {
        SignalBase* sig;
        TriggerType edge;
        std::coroutine_handle<> handle;
        WaitId wid;
    };
    std::vector<MonitorEntry> monitor_queue_;
    std::vector<MonitorEntry> pending_monitor_;
    bool monitor_processing_ = false;

    std::vector<std::function<void()>> delta_callbacks_;
    std::vector<std::function<void()>> tick_hooks_;
    std::vector<std::function<void()>> pre_eval_hooks_;
    std::vector<std::function<void()>> post_eval_hooks_;
    std::vector<std::function<void()>> commit_eval_hooks_;
    std::vector<std::function<void()>> tick_end_hooks_;

    std::function<void()> eval_fn_;
    std::function<void(sim_time)> dump_fn_;

    sim_time now_ = 0;
    size_t max_delta_ = 500;
};

} // namespace corosim
