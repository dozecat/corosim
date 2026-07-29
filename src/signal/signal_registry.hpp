/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        signal_registry.hpp
 * @brief       Tracks registered signals and commits pending NBA updates
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Owns the signal list and pending queue used by the scheduler
 *              each tick.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <vector>
#include "signal_base.hpp"

namespace corosim {

/**
 * @brief Central registry for SignalBase instances.
 *
 * Pending NBA values are committed via commit_all(); dirty/edge flags are
 * queried by the scheduler after each eval/delta phase.
 */
class SignalRegistry {
public:
    /** @brief Register @p s for commit and edge tracking. */
    void register_signal(SignalBase* s);
    /** @brief Unregister @p s from all lists. */
    void unregister_signal(SignalBase* s);
    /** @brief Queue @p s for NBA commit at end of phase. */
    void mark_pending(SignalBase* s);

    /** @brief Apply all pending next() values. */
    void commit_all();
    /** @brief True if any signal is dirty after the last commit. */
    bool any_dirty() const;
    void clear_dirty();
    void clear_edge_flags();

    /** @brief All registered signals (not only dirty ones). */
    std::vector<SignalBase*>& signals() { return signals_; }
    const std::vector<SignalBase*>& signals() const { return signals_; }

private:
    std::vector<SignalBase*> signals_;
    std::vector<SignalBase*> pending_signals_;
};

} // namespace corosim
