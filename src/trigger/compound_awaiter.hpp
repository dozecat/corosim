#pragma once

#include <array>
#include <coroutine>
#include <utility>

#include "../core/types.hpp"
#include "internal.hpp"
#include "edge_awaiter.hpp"
#include "delay_awaiter.hpp"

namespace corosim {

template <typename... Triggers>
auto any(Triggers&&... triggers) {
    std::array<TriggerInfo, sizeof...(Triggers)> infos = {{triggers.trigger_info()...}};

    struct Awaiter {
        std::array<TriggerInfo, sizeof...(Triggers)> infos;
        int which_ = -1;

        bool await_ready() noexcept { return false; }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            detail::register_compound_wait(h, infos.data(), infos.size(), &which_);
        }

        int await_resume() noexcept { return which_; }
    };

    return Awaiter{infos};
}

} // namespace corosim
