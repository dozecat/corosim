/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        helpers.hpp
 * @brief       Factory helpers for edge and change triggers
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Builds EdgeAwaiter for posedge, negedge, and change on
 *              Signal<T>.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include "signal/signal.hpp"
#include "trigger/edge.hpp"

namespace corosim {

/** @brief Rising-edge awaiter; @p T must be a 1-byte signal type. */
template <typename T>
EdgeAwaiter posedge(Signal<T>& sig) {
    static_assert(sizeof(T) <= 1, "posedge requires 1-byte signal type");
    return {sig.base_ptr(), TriggerInfo::POSEDGE};
}

/** @brief Falling-edge awaiter; @p T must be a 1-byte signal type. */
template <typename T>
EdgeAwaiter negedge(Signal<T>& sig) {
    static_assert(sizeof(T) <= 1, "negedge requires 1-byte signal type");
    return {sig.base_ptr(), TriggerInfo::NEGEDGE};
}

/** @brief Value-change awaiter for any Signal<T>. */
template <typename T>
EdgeAwaiter change(Signal<T>& sig) {
    return {sig.base_ptr(), TriggerInfo::CHANGE};
}

} // namespace corosim
