/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        signal_base.hpp
 * @brief       Abstract base for simulation signals
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Defines commit, dirty, and edge-query hooks consumed by the
 *              scheduler.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <cstdint>

namespace corosim {

/** @brief Polymorphic signal interface for registry and scheduler. */
class SignalBase {
public:
    virtual ~SignalBase() = default;
    /** @brief Apply pending NBA value and update dirty/edge flags. */
    virtual void commit() = 0;
    virtual bool is_dirty() const = 0;
    virtual void clear_dirty() = 0;
    virtual bool has_posedge() const = 0;
    virtual bool has_negedge() const = 0;
    virtual bool has_changed() const = 0;
    virtual void clear_edge_flags() = 0;
};

} // namespace corosim
