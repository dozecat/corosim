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
}

void SignalRegistry::mark_pending(SignalBase* s) {
    pending_signals_.push_back(s);
}

/** @brief Commit every pending NBA, then clear the pending queue. */
void SignalRegistry::commit_all() {
    for (auto* sig : pending_signals_) {
        if (sig) sig->commit();
    }
    pending_signals_.clear();
}

bool SignalRegistry::any_dirty() const {
    for (auto* sig : signals_) {
        if (sig && sig->is_dirty()) return true;
    }
    return false;
}

void SignalRegistry::clear_dirty() {
    for (auto* sig : signals_) {
        if (sig) sig->clear_dirty();
    }
}

void SignalRegistry::clear_edge_flags() {
    for (auto* sig : signals_) {
        if (sig) sig->clear_edge_flags();
    }
}

} // namespace corosim
