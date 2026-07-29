/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        any.hpp
 * @brief       Wait until the first of several triggers fires
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Returns the index of the winning trigger; cancels the remaining
 *              waits.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <array>
#include <coroutine>
#include <utility>

#include "core/types.hpp"
#include "core/detail/context.hpp"
#include "trigger/edge.hpp"
#include "trigger/delay.hpp"

namespace corosim {

/**
 * @brief Await the first of @p triggers to fire.
 * @return Index of the winning trigger (0-based).
 */
template <typename... Triggers>
auto any(Triggers&&... triggers) {
    std::array<TriggerInfo, sizeof...(Triggers)> infos = {{triggers.trigger_info()...}};

    struct Awaiter {
        std::array<TriggerInfo, sizeof...(Triggers)> infos_;
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
