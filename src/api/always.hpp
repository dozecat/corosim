#pragma once

#include <functional>
#include <memory>
#include "../core/types.hpp"
#include "../core/kernel.hpp"
#include "../trigger/delay_awaiter.hpp"

namespace corosim {

namespace detail {

struct AlwaysEdgeProc {
    SignalBase* sig;
    TriggerType edge;

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_wait(sig, edge, h);
    }

    void await_resume() noexcept {}
};

template <typename Fn>
static Proc make_always_edge_coro(SignalBase* sig, TriggerType edge, Fn fn) {
    while (true) {
        co_await AlwaysEdgeProc{sig, edge};
        fn();
    }
}

template <typename Fn>
static Proc make_always_delay_coro(sim_time interval, Fn fn) {
    // First fire immediately (at t=0 during init resume)
    fn();
    while (true) {
        co_await delay(interval);
        fn();
    }
}

} // namespace detail

template <typename Trigger, typename Fn>
void always(Trigger t, Fn fn) {
    auto* k = detail::current_kernel();
    if (!k) return;

    auto info = t.trigger_info();

    if (info.type == TriggerType::DELAY) {
        auto interval = info.interval;
        k->add_process([interval, fn = std::move(fn)]() -> Proc {
            return detail::make_always_delay_coro(interval, std::move(fn));
        });
    } else {
        SignalBase* sig = info.sig;
        TriggerType edge = info.type;
        k->add_process([sig, edge, fn = std::move(fn)]() -> Proc {
            return detail::make_always_edge_coro(sig, edge, std::move(fn));
        });
    }
}

} // namespace corosim
