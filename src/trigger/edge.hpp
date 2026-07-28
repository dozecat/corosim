#pragma once

#include <coroutine>
#include "core/types.hpp"
#include "signal/signal_base.hpp"
#include "core/detail/context.hpp"

namespace corosim {

struct EdgeAwaiter {
    SignalBase* sig_;
    TriggerType type_;

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_wait(sig_, type_, h);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {type_, sig_, 0};
    }
};

} // namespace corosim
