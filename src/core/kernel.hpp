#pragma once

#include <coroutine>
#include <functional>
#include <memory>
#include <cassert>

#include "core/types.hpp"
#include "signal/signal_registry.hpp"
#include "trigger/spec.hpp"
#include "scheduler/scheduler.hpp"
#include "coroutine/coroutine_manager.hpp"
#include "coroutine/task.hpp"

namespace corosim {

/**
 * @brief Core simulation engine facade.
 *
 * Owns SignalRegistry, Scheduler, and CoroutineManager. Template methods are
 * defined in kernel_impl.hpp.
 */
class Kernel {
public:
    Kernel();
    ~Kernel();

    SignalRegistry& signals() { return signals_; }
    Scheduler& scheduler() { return sched_; }
    CoroutineManager& coroutine_manager() { return coroutine_manager_; }

    void set_top(void* top) { top_ = top; }
    void set_verilator_time(sim_time t);

    sim_time now() const { return sched_.now(); }
    bool triggered(SignalVal* sig, TriggerType t) const { return sched_.triggered(sig, t); }

    /** @brief Spawn a repeating coroutine on @p t (signal trigger or delay). */
    template <typename TriggerT, typename Fn>
    void always(TriggerT t, Fn fn);

    /** @brief Spawn a one-shot coroutine. */
    template <typename Fn, typename... Args>
    Coroutine* instance(Fn&& fn, Args&&... args);

    /** @brief Run @p fn on trigger before DUT eval (sample phase). */
    template <typename TriggerT, typename Fn>
    void sample(TriggerT t, Fn fn);

    /** @brief Run @p fn on trigger after DUT eval (drive phase). */
    template <typename TriggerT, typename Fn>
    void drive(TriggerT t, Fn fn);

    /** @brief Run until @p duration or no pending timers. */
    void run(sim_time duration);

    /** @name Awaiter registration (internal) */
    ///@{
    void register_trigger_wait(SignalVal* sig, TriggerType t, std::coroutine_handle<> h,
                               int fire_idx = -1, int* fired = nullptr);
    void register_delay_wait(std::coroutine_handle<> h, sim_time interval,
                             int fire_idx = -1, int* fired = nullptr);
    ///@}

private:
    template <typename TriggerT, typename Fn>
    Coroutine* spawn_repeating(TriggerT t, Fn fn);

    template <typename Fn>
    Coroutine* add_coroutine(Fn&& fn);

    SignalRegistry signals_;
    Scheduler sched_;
    CoroutineManager coroutine_manager_;
    void* top_ = nullptr;
};

} // namespace corosim
