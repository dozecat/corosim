#pragma once

#include <type_traits>
#include <variant>

#include "core/kernel.hpp"
#include "trigger/trigger_awaiter.hpp"
#include "trigger/delay.hpp"

namespace corosim::detail {

template <typename Fn>
inline Task make_always_trigger_coro(SignalVal* sig, TriggerType t, Fn fn) {
    while (true) {
        co_await TriggerAwaiter{sig, t};
        fn();
    }
}

template <typename Fn>
inline Task make_always_delay_coro(sim_time interval, Fn fn) {
    while (true) {
        co_await delay(interval);
        fn();
    }
}

} // namespace corosim::detail

namespace corosim {

template <typename TriggerT, typename Fn>
Coroutine* Kernel::spawn_repeating(TriggerT t, Fn fn) {
    auto info = t.trigger_info();
    return std::visit([&](const auto& spec) -> Coroutine* {
        using T = std::decay_t<decltype(spec)>;
        if constexpr (std::is_same_v<T, DelaySpec>) {
            return add_coroutine([interval = spec.interval, fn = std::move(fn)]() -> Task {
                return detail::make_always_delay_coro(interval, std::move(fn));
            });
        } else {
            return add_coroutine([sig = spec.sig, type = spec.type, fn = std::move(fn)]() -> Task {
                return detail::make_always_trigger_coro(sig, type, std::move(fn));
            });
        }
    }, info);
}

template <typename TriggerT, typename Fn>
void Kernel::always(TriggerT t, Fn fn) {
    static_assert(std::is_void_v<std::invoke_result_t<Fn&>>,
                  "always callback must return void; use instance() for coroutine processes");
    spawn_repeating(t, std::move(fn));
}

template <typename Fn, typename... Args>
Coroutine* Kernel::instance(Fn&& fn, Args&&... args) {
    static_assert(std::is_same_v<std::invoke_result_t<Fn&, Args&...>, Task>,
                  "instance callable must return Task");
    return add_coroutine([fn = std::forward<Fn>(fn), ...args = std::forward<Args>(args)]() -> Task {
        return fn(args...);
    });
}

template <typename TriggerT, typename Fn>
void Kernel::sample(TriggerT t, Fn fn) {
    static_assert(std::is_void_v<std::invoke_result_t<Fn&>>,
                  "sample callback must return void");
    auto info = t.trigger_info();
    std::visit([&](const auto& spec) {
        using T = std::decay_t<decltype(spec)>;
        if constexpr (std::is_same_v<T, TriggerSpec>) {
            sched_.on_pre_eval([this, sig = spec.sig, type = spec.type, fn = std::move(fn)] {
                if (sched_.triggered(sig, type)) {
                    fn();
                }
            });
        }
    }, info);
}

template <typename TriggerT, typename Fn>
void Kernel::drive(TriggerT t, Fn fn) {
    static_assert(std::is_void_v<std::invoke_result_t<Fn&>>,
                  "drive callback must return void");
    auto info = t.trigger_info();
    std::visit([&](const auto& spec) {
        using T = std::decay_t<decltype(spec)>;
        if constexpr (std::is_same_v<T, TriggerSpec>) {
            sched_.on_post_eval([this, sig = spec.sig, type = spec.type, fn = std::move(fn)] {
                if (sched_.triggered(sig, type)) {
                    fn();
                }
            });
        }
    }, info);
}

template <typename Fn>
Coroutine* Kernel::add_coroutine(Fn&& fn) {
    auto* c = coroutine_manager_.spawn(std::forward<Fn>(fn));
    auto& p = Task::promise_type::from_handle(c->void_handle());
    p.kernel = this;
    p.coroutine = c;
    c->resume();
    return c;
}

} // namespace corosim
