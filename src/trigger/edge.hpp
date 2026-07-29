/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        edge.hpp
 * @brief       Coroutine awaiter for signal edge or change
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Suspends until the scheduler observes the requested edge on a
 *              SignalBase.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include "core/types.hpp"
#include "signal/signal_base.hpp"
#include "core/detail/context.hpp"

namespace corosim {

/** @brief Awaitable for POSEDGE / NEGEDGE / CHANGE on a signal. */
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
