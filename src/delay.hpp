/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        delay.hpp
 * @brief       Simulation time type and Delay awaiter/trigger
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Defines sim_time as the base time unit and the Delay struct
 *              which serves both as a trigger (sim.always(delay(n), ...))
 *              and as an awaiter (co_await delay(n)).
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/25  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <cstdint>

namespace corosim {

using sim_time = uint64_t;

class Engine;

namespace detail {
void register_delay_wakeup(std::coroutine_handle<> h, sim_time interval);
}

struct Delay {
    sim_time interval;

    Delay(sim_time n) : interval(n) {}

    bool await_ready() const noexcept { return interval == 0; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_delay_wakeup(h, interval);
    }

    void await_resume() noexcept {}
};

inline Delay delay(sim_time n) { return Delay(n); }

} // namespace corosim
