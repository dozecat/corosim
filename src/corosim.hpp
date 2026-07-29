/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        corosim.hpp
 * @brief       Umbrella header for the corosim public API
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Includes core, signal, trigger, and process headers plus Kernel
 *              template implementations.
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
#include "core/sim.hpp"
#include "core/module.hpp"
#include "core/dut.hpp"

// Signal
#include "signal/signal_base.hpp"
#include "signal/signal_registry.hpp"
#include "signal/signal.hpp"
#include "signal/wide.hpp"
#include "signal/helpers.hpp"

// Trigger
#include "trigger/edge.hpp"
#include "trigger/delay.hpp"
#include "trigger/any.hpp"

// Process
#include "process/task.hpp"
#include "process/process.hpp"
#include "process/process_manager.hpp"

// Template implementations (must be included after all declarations)
#include "core/kernel_impl.hpp"
