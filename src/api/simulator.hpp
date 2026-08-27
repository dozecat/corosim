#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "core/types.hpp"
#include "core/kernel.hpp"
#include "signal/signal.hpp"
#include "trigger/delay.hpp"
#include "trigger/helpers.hpp"
#include "coroutine/coroutine.hpp"

namespace corosim {

/**
 * @brief High-level simulation facade over a Verilator @p TOP.
 * @tparam TOP Verilator-generated top module type.
 */
template <typename TOP>
class Simulator {
    TOP& top_;
    Kernel kernel_;
    std::vector<std::unique_ptr<SignalVal>> owned_signals_;
    bool dump_setup_ = false;

public:
    explicit Simulator(TOP& top) : top_(top), kernel_() {
        kernel_.set_top(&top);
        kernel_.scheduler().set_eval_fn([&top] { top.eval(); });
        kernel_.scheduler().set_time_hook([this](sim_time t) { kernel_.set_verilator_time(t); });
    }

    /** @brief Bind a Verilator field as a Signal. */
    template <typename T>
    Signal<T>& sig(T& field) {
        auto s = std::make_unique<Signal<T>>(kernel_.signals(), &field);
        auto* ptr = s.get();
        owned_signals_.push_back(std::move(s));
        return *ptr;
    }

    /** @brief Create a software-only Signal (no DUT binding). */
    template <typename T>
    Signal<T>& sig() {
        auto s = std::make_unique<Signal<T>>(kernel_.signals());
        auto* ptr = s.get();
        owned_signals_.push_back(std::move(s));
        return *ptr;
    }

    /** @brief Toggle @p sig every period/2 time units. */
    template <typename T>
    void clock(Signal<T>& sig, sim_time period) {
        kernel_.always(corosim::delay(period / 2), [p = &sig] { p->next(!p->read()); });
    }

    SignalRegistry& signals() { return kernel_.signals(); }

    sim_time now() const { return kernel_.now(); }

    template <typename TriggerT, typename Fn>
    void always(TriggerT t, Fn fn) { kernel_.always(t, std::move(fn)); }

    template <typename Fn, typename... Args>
    Coroutine* instance(Fn&& fn, Args&&... args) {
        return kernel_.instance(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }

    template <typename TriggerT, typename Fn>
    Coroutine* check(TriggerT t, Fn fn) {
        return kernel_.check(t, std::move(fn));
    }

    template <typename TriggerT, typename Fn>
    void sample(TriggerT t, Fn fn) { kernel_.sample(t, std::move(fn)); }

    template <typename TriggerT, typename Fn>
    void drive(TriggerT t, Fn fn) { kernel_.drive(t, std::move(fn)); }

    template <typename Fn>
    void pre_eval(Fn&& fn) { kernel_.pre_eval(std::forward<Fn>(fn)); }

    template <typename Fn>
    void post_eval(Fn&& fn) { kernel_.post_eval(std::forward<Fn>(fn)); }

    /**
     * @brief Run simulation up to @p duration.
     * @param dump_fn Optional per-tick dump callback (e.g. VCD).
     */
    void run(sim_time duration, std::function<void(sim_time)> dump_fn = nullptr) {
        if (dump_fn && !dump_setup_) {
            kernel_.scheduler().set_dump_fn([dump_fn](sim_time t) { dump_fn(t); });
            dump_setup_ = true;
        }
        kernel_.run(duration);
    }
};

} // namespace corosim
