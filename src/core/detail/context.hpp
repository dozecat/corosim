/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        context.hpp
 * @brief       Internal awaiter registration bridge to Kernel
 * @see         https://github.com/dozecat/corosim
 *
 * @details     detail:: helpers resolve the Kernel from a coroutine handle and
 *              register waits.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <cstddef>
#include "core/types.hpp"

namespace corosim {

class Kernel;
class SignalBase;

namespace detail {

Kernel* get_kernel(std::coroutine_handle<> h);

void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx = -1, int* fired = nullptr);
void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx = -1, int* fired = nullptr);
void register_compound_wait(std::coroutine_handle<> h, const TriggerInfo* infos, size_t count, int* fired = nullptr);

} // namespace detail

} // namespace corosim
