/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        wait.hpp
 * @brief       Per-process active wait tracking
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Stable WaitId entries so the scheduler can invalidate competing
 *              any() waits.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <list>
#include "core/types.hpp"

namespace corosim {

/**
 * @brief Tracks outstanding waits for one Process.
 *
 * Returned WaitId* pointers are list-backed and remain stable until cancelled.
 */
class WaitGroup {
public:
    explicit WaitGroup(ProcessId owner) : owner_(owner) {}

    /** @brief Add an edge watch; returns stable WaitId pointer. */
    WaitId* add_edge_watch(SignalBase* sig, TriggerType edge);
    /** @brief Add a delay watch; returns stable WaitId pointer. */
    WaitId* add_delay_watch(TimerId timer);

    void cancel_all();
    /** @brief Invalidate all waits except @p keep (used by any()). */
    void cancel_others(WaitId* keep);

    bool any_active() const;

private:
    struct Entry {
        WaitId id;
        bool active = true;
    };

    ProcessId owner_;
    std::list<Entry> entries_;
    uint64_t next_id_ = 0;
};

} // namespace corosim
