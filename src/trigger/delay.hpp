#pragma once

#include <coroutine>

#include "check.hpp"
#include "trigger/spec.hpp"
#include "core/detail/wait_register.hpp"

namespace corosim {

/** @brief Awaitable / trigger that fires after @p interval_ time units. */
struct Delay {
    sim_time interval_;

    explicit Delay(sim_time n) : interval_(n) {
        COROSIM_ASSERT(interval_ > 0, "delay interval must be greater than 0");
    }

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_delay_wait(h, interval_);
    }

    void await_resume() noexcept {}

    Trigger trigger_info() const { return DelaySpec{interval_}; }
};

/** @brief Construct a Delay of @p n time units. */
inline Delay delay(sim_time n) { return Delay(n); }

} // namespace corosim
