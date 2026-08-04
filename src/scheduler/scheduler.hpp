#pragma once

#include <coroutine>
#include <functional>
#include "core/types.hpp"
#include "signal/signal_base.hpp"
#include "signal/signal_registry.hpp"
#include "scheduler/timer_engine.hpp"
#include "scheduler/monitor_engine.hpp"
#include "scheduler/delta_engine.hpp"

namespace corosim {

class Scheduler {
public:
    explicit Scheduler(SignalRegistry& signals)
        : signals_(signals), delta_(signals) {}

    void set_eval_fn(std::function<void()> fn) { delta_.set_eval_fn(std::move(fn)); }
    void set_dump_fn(std::function<void(sim_time)> fn) { dump_fn_ = std::move(fn); }
    void set_cancel_fn(std::function<void(std::coroutine_handle<>, WaitId*)> fn) { cancel_others_fn_ = std::move(fn); }

    sim_time now() const { return timer_.now(); }

    template <typename Fn>
    void on_pre_eval(Fn&& fn) { delta_.on_pre_eval(std::forward<Fn>(fn)); }
    template <typename Fn>
    void on_post_eval(Fn&& fn) { delta_.on_post_eval(std::forward<Fn>(fn)); }

    bool had_edge(SignalBase* sig, TriggerType edge) const { return delta_.had_edge(sig, edge); }

    TimerId schedule_timer(sim_time deadline, std::coroutine_handle<> h, WaitId* wid,
                           int fire_idx = -1, int* fired = nullptr);
    void schedule_monitor(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h,
                          WaitId* wid, int fire_idx = -1, int* fired = nullptr);

    void run(sim_time duration);

    void on_signal_destroy(SignalBase* sig) { monitor_.unwatch_all(sig); }

private:
    void run_one_tick();
    void fire_coroutine(std::coroutine_handle<> h, WaitId* wid, int* fire_idx, int fire_value);

    SignalRegistry& signals_;
    TimerEngine timer_;
    MonitorEngine monitor_;
    DeltaEngine delta_;

    std::function<void(std::coroutine_handle<>, WaitId*)> cancel_others_fn_;
    std::function<void(sim_time)> dump_fn_;
};

} // namespace corosim
