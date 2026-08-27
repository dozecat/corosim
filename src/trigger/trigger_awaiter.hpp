#pragma once

#include <coroutine>

#include "trigger/spec.hpp"
#include "core/detail/wait_register.hpp"

namespace corosim {

/** @brief Awaitable for a signal trigger (posedge / negedge / change). */
struct TriggerAwaiter {
    SignalVal* sig_;
    TriggerType type_;

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_trigger_wait(sig_, type_, h);
    }

    void await_resume() noexcept {}

    Trigger trigger_info() const { return TriggerSpec{sig_, type_}; }
};

} // namespace corosim
