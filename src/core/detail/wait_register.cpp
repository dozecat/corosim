/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        context.cpp
 * @brief       Awaiter registration helper implementations
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Looks up Kernel/Process from handles and schedules edge, delay,
 *              or compound waits.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#include "wait_register.hpp"
#include "core/kernel.hpp"
#include "process/task.hpp"

namespace corosim {

namespace detail {

Kernel* get_kernel(std::coroutine_handle<> h) {
    return Task::promise_type::from_handle(h).kernel;
}

void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx, int* fired) {
    auto* k = get_kernel(h);
    if (!k) return;
    k->register_edge_wait(sig, edge, h, fire_idx, fired);
}

void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx, int* fired) {
    auto* k = get_kernel(h);
    if (!k) return;
    k->register_delay_wait(h, interval, fire_idx, fired);
}

/**
 * @brief Register every trigger in @p infos against the same coroutine.
 * @param fired Written with the winning trigger index when one fires.
 */
void register_compound_wait(std::coroutine_handle<> h, const TriggerInfo* infos, size_t count, int* fired) {
    auto* k = get_kernel(h);
    if (!k) return;
    for (size_t i = 0; i < count; ++i) {
        switch (infos[i].type) {
        case TriggerInfo::POSEDGE:
        case TriggerInfo::NEGEDGE:
        case TriggerInfo::CHANGE:
            k->register_edge_wait(infos[i].sig, infos[i].type, h, (int)i, fired);
            break;
        case TriggerInfo::DELAY:
            k->register_delay_wait(h, infos[i].interval, (int)i, fired);
            break;
        }
    }
}

} // namespace detail

} // namespace corosim
