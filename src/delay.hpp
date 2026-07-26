#pragma once

#include <coroutine>
#include "types.hpp"

namespace corosim {

class Engine;

namespace detail {
void register_delay_wakeup(std::coroutine_handle<> h, sim_time interval);
}

struct Delay {
    friend class Engine;

    Delay(sim_time n) : interval_(n) {}

    bool await_ready() const noexcept { return interval_ == 0; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_delay_wakeup(h, interval_);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerInfo::DELAY, nullptr, interval_};
    }

private:
    sim_time interval_;
};

inline Delay delay(sim_time n) { return Delay(n); }

} // namespace corosim
