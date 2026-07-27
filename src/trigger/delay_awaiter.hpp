#pragma once

#include <coroutine>
#include "../core/types.hpp"
#include "internal.hpp"

namespace corosim {

struct Delay {
    sim_time interval_;

    explicit Delay(sim_time n) : interval_(n) {}

    bool await_ready() const noexcept { return interval_ == 0; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_delay_wait(h, interval_);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerType::DELAY, nullptr, interval_};
    }
};

inline Delay delay(sim_time n) { return Delay(n); }

} // namespace corosim
