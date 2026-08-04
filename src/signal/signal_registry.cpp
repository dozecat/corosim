/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        signal_registry.cpp
 * @brief       SignalRegistry method implementations
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Implements register/unregister, pending commit, and dirty/edge
 *              flag helpers.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#include "signal_registry.hpp"
#include <algorithm>

namespace corosim {

void SignalRegistry::register_signal(SignalBase* s) {
    signals_.push_back(s);
}

void SignalRegistry::unregister_signal(SignalBase* s) {
    auto it = std::find(signals_.begin(), signals_.end(), s);
    if (it != signals_.end()) signals_.erase(it);
    auto pit = std::find(pending_signals_.begin(), pending_signals_.end(), s);
    if (pit != pending_signals_.end()) pending_signals_.erase(pit);
    auto cit = std::find(changed_signals_.begin(), changed_signals_.end(), s);
    if (cit != changed_signals_.end()) changed_signals_.erase(cit);
    if (on_signal_unregistered) on_signal_unregistered(s);
}

void SignalRegistry::mark_pending(SignalBase* s) {
    pending_signals_.push_back(s);
}

/** @brief Record a value change; added once per tick via a per-signal flag. */
void SignalRegistry::mark_changed(SignalBase* s) {
    if (s->marked_changed()) return;
    s->set_marked_changed(true);
    changed_signals_.push_back(s);
}

/** @brief Commit every pending NBA, then clear the pending queue. */
void SignalRegistry::commit_all() {
    for (auto* sig : pending_signals_) {
        if (sig) sig->commit();
    }
    pending_signals_.clear();
}

/** @brief Clear edge flags on only the signals that changed this tick. */
void SignalRegistry::clear_edge_flags() {
    for (auto* sig : changed_signals_) {
        if (sig) {
            sig->clear_edge_flags();
            sig->set_marked_changed(false);
        }
    }
    changed_signals_.clear();
}

} // namespace corosim
