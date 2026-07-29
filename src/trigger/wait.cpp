/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        wait.cpp
 * @brief       WaitGroup method implementations
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Adds edge/delay watches and cancels active wait entries.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#include "trigger/wait.hpp"

namespace corosim {

WaitId* WaitGroup::add_edge_watch(SignalBase* sig, TriggerType edge) {
    entries_.push_back({WaitId{++next_id_, owner_.generation, edge, sig}, true});
    return &entries_.back().id;
}

WaitId* WaitGroup::add_delay_watch(TimerId timer) {
    entries_.push_back({WaitId{++next_id_, owner_.generation, TriggerType::DELAY, timer}, true});
    return &entries_.back().id;
}

/**
 * @brief Invalidate all waits.
 * @note Entries are never erased; the scheduler may still hold WaitId*.
 */
void WaitGroup::cancel_all() {
    for (auto& e : entries_) {
        e.id.invalidate();
    }
}

/**
 * @brief Invalidate all waits except @p keep (any() winner).
 * @note Entries are never erased; the scheduler may still hold WaitId*.
 */
void WaitGroup::cancel_others(WaitId* keep) {
    for (auto& e : entries_) {
        if (&e.id != keep) {
            e.id.invalidate();
        }
    }
}

bool WaitGroup::any_active() const {
    for (auto& e : entries_) {
        if (e.id.valid()) return true;
    }
    return false;
}

} // namespace corosim
