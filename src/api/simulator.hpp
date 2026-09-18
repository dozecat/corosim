#pragma once

#include <functional>
#include <memory>
#include <vector>

#include "core/types.hpp"
#include "core/kernel.hpp"
#include "check.hpp"
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

    /**
     * @brief Generate a clock with a full @p period.
     * @param start_high Initial clock level. Defaults to low.
     *
     * Odd periods alternate floor(period / 2) and ceil(period / 2) half-intervals.
     */
    template <typename T>
    void clock(Signal<T>& sig, sim_time period, bool start_high = false) {
        static_assert(sizeof(T) <= 1, "clock requires a 1-byte signal type");
        COROSIM_ASSERT(period >= 2, "clock period must be at least 2 time units");

        kernel_.instance([p = &sig, period, start_high]() -> Task {
            bool high = start_high;
            p->next(high ? 1 : 0);

            const sim_time high_time = period / 2;
            const sim_time low_time = period - high_time;
            while (true) {
                co_await delay(high ? high_time : low_time);
                high = !high;
                p->next(high ? 1 : 0);
            }
        });
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
    void sample(TriggerT t, Fn fn) { kernel_.sample(t, std::move(fn)); }

    template <typename TriggerT, typename Fn>
    void drive(TriggerT t, Fn fn) { kernel_.drive(t, std::move(fn)); }

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
