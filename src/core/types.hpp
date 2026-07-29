/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        types.hpp
 * @brief       Core time, trigger, process, and wait identifiers
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Shared POD types used across scheduler, signals, and coroutine
 *              waits.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <variant>

namespace corosim {

/** @brief Simulation time in discrete time units. */
using sim_time = uint64_t;

class SignalBase;

/** @brief Describes a delay or edge/change trigger. */
struct TriggerInfo {
    enum Type : int { DELAY = 0, POSEDGE, NEGEDGE, CHANGE };
    Type type;
    SignalBase* sig;   ///< Signal for edge/change; nullptr for DELAY.
    sim_time interval; ///< Delay interval; unused for edge/change.
};

using TriggerType = TriggerInfo::Type;

/** @brief Opaque timer handle in the scheduler timed queue. */
struct TimerId { uint64_t id; };

using WaitTarget = std::variant<SignalBase*, TimerId>;

/** @brief Process identity; generation bumps on reuse/restart. */
struct ProcessId {
    uint64_t id;
    uint32_t generation;

    bool operator==(const ProcessId& o) const = default;
    bool operator!=(const ProcessId& o) const = default;
};

/**
 * @brief Active wait token for a process.
 *
 * id == 0 means invalidated (e.g. sibling wait cancelled by any()).
 */
struct WaitId {
    uint64_t id;
    uint32_t process_generation;
    TriggerType type;
    WaitTarget target;

    bool valid() const { return id != 0; }
    void invalidate() { id = 0; }
};

} // namespace corosim
