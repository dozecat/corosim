#pragma once

#include <functional>
#include <memory>
#include "../core/types.hpp"
#include "../core/kernel.hpp"
#include "../trigger/edge_awaiter.hpp"
#include "../trigger/delay_awaiter.hpp"
#include "../trigger/compound_awaiter.hpp"

namespace corosim {

namespace detail {

// Helper: create a repeating always-coroutine Process from raw signal info.
// Called INSIDE Kernel::add_process context, where the coroutine lambda
// is directly in the same stack frame, avoiding std::function/forwarding issues.
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

} // namespace detail

template <typename Trigger, typename Fn>
void always(Trigger t, Fn fn) {
    auto* k = detail::current_kernel();
    if (!k) return;

    auto info = t.trigger_info();

    if (info.type == TriggerType::DELAY) {
        auto interval = info.interval;
        auto next = std::make_shared<sim_time>(0);
        k->on_tick([=]() mutable {
            if (k->now() >= *next) {
                fn();
                *next = k->now() + interval;
            }
        });
    } else {
        SignalBase* sig = info.sig;
        TriggerType edge = info.type;
        // Pass the factory lambda — std::function wrapping is safe here
        // because make_always_edge_coro creates the Proc within the same full-expression.
        k->add_process([sig, edge, fn = std::move(fn)]() -> Proc {
            return detail::make_always_edge_coro(sig, edge, std::move(fn));
        });
    }
}

} // namespace corosim
