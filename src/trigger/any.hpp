#pragma once

#include <array>
#include <coroutine>
#include <utility>

#include "trigger/spec.hpp"
#include "core/detail/wait_register.hpp"

namespace corosim {

/**
 * @brief Await the first of @p triggers to fire.
 * @return Index of the winning trigger (0-based).
 */
template <typename... Triggers>
auto any(Triggers&&... triggers) {
    std::array<Trigger, sizeof...(Triggers)> infos = {{triggers.trigger_info()...}};

    struct Awaiter {
        std::array<Trigger, sizeof...(Triggers)> infos_;
        int which_ = -1;

        bool await_ready() noexcept { return false; }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            detail::register_compound_wait(h, infos_.data(), infos_.size(), &which_);
        }

        int await_resume() noexcept { return which_; }
    };

    return Awaiter{std::move(infos)};
}

} // namespace corosim
