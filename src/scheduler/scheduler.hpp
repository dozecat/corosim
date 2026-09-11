#pragma once

#include <coroutine>
#include <functional>
#include <memory>

#include "core/types.hpp"
#include "signal/signal_val.hpp"
#include "signal/signal_registry.hpp"
#include "trigger/spec.hpp"
#include "coroutine/wait.hpp"
#include "scheduler/fire_ticket.hpp"
#include "scheduler/timer_engine.hpp"
#include "scheduler/monitor_engine.hpp"
#include "scheduler/delta_engine.hpp"

namespace corosim {

class Scheduler {
public:
    explicit Scheduler(SignalRegistry& signals)
        : delta_(signals) {}

    void set_eval_fn(std::function<void()> fn) { delta_.set_eval_fn(std::move(fn)); }
    void set_dump_fn(std::function<void(sim_time)> fn) { dump_fn_ = std::move(fn); }
    void set_time_hook(std::function<void(sim_time)> fn) { time_hook_ = std::move(fn); }
    void set_cancel_fn(std::function<void(std::coroutine_handle<>, const std::shared_ptr<WaitToken>&)> fn) {
        cancel_others_fn_ = std::move(fn);
    }

    sim_time now() const { return timer_.now(); }

    template <typename Fn>
    void on_pre_eval(Fn&& fn) { delta_.on_pre_eval(std::forward<Fn>(fn)); }
    template <typename Fn>
    void on_post_eval(Fn&& fn) { delta_.on_post_eval(std::forward<Fn>(fn)); }

    bool triggered(SignalVal* sig, TriggerType t) const { return delta_.triggered(sig, t); }

    void schedule_timer(sim_time deadline, std::coroutine_handle<> h, std::shared_ptr<WaitToken> token,
                        int fire_idx = -1, int* fired = nullptr) {
        timer_.schedule(deadline, h, std::move(token), fire_idx, fired);
    }
    void schedule_monitor(SignalVal* sig, TriggerType t, std::coroutine_handle<> h,
                          std::shared_ptr<WaitToken> token,
                          int fire_idx = -1, int* fired = nullptr) {
        monitor_.watch(sig, t, h, std::move(token), fire_idx, fired);
    }

    void run(sim_time duration);

    void on_signal_destroy(SignalVal* sig) { monitor_.unwatch_all(sig); }

private:
    void run_one_tick();
    void fire_coroutine(const FireTicket& t);

    TimerEngine timer_;
    MonitorEngine monitor_;
    DeltaEngine delta_;
    std::function<void(std::coroutine_handle<>, const std::shared_ptr<WaitToken>&)> cancel_others_fn_;
    std::function<void(sim_time)> dump_fn_;
    std::function<void(sim_time)> time_hook_;
};

} // namespace corosim
