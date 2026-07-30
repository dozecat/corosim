/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        delay.hpp
 * @brief       Coroutine awaiter and trigger for timed delays
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Suspends for N time units; also usable as an always() trigger.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include "core/types.hpp"
#include "core/detail/wait_register.hpp"

namespace corosim {

/** @brief Awaitable / trigger that fires after @p interval_ time units. */
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

/** @brief Construct a Delay of @p n time units. */
inline Delay delay(sim_time n) { return Delay(n); }

} // namespace corosim
