/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        corosim.hpp
 * @brief       Umbrella header for the corosim public API
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Includes core, api, signal, trigger, and coroutine headers plus
 *              Kernel template implementations.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

// Core
#include "core/types.hpp"
#include "core/kernel.hpp"

// API (public facades)
#include "api/simulator.hpp"
#include "api/module.hpp"
#include "api/dut.hpp"

// Signal
#include "signal/signal_val.hpp"
#include "signal/signal_registry.hpp"
#include "signal/signal.hpp"
#include "signal/wide.hpp"

// Trigger
#include "trigger/spec.hpp"
#include "trigger/trigger_awaiter.hpp"
#include "trigger/delay.hpp"
#include "trigger/any.hpp"
#include "trigger/helpers.hpp"

// Coroutine
#include "coroutine/task.hpp"
#include "coroutine/coroutine.hpp"
#include "coroutine/coroutine_manager.hpp"

// Verification
#include "check.hpp"
#include "logger.hpp"

// Template implementations (must be included after all declarations)
#include "core/kernel_impl.hpp"
