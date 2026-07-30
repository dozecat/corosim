/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        process.cpp
 * @brief       Process method implementations
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Implements resume and cancel for coroutine processes.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#include "process.hpp"

namespace corosim {

Process::~Process() {
    wait_group_.cancel_all();
}

/** @brief Resume the coroutine; mark DONE or WAITING afterwards. */
void Process::resume() {
    if (state_ == ProcessState::CANCELLED || state_ == ProcessState::DONE) return;
    task_.resume();
    if (task_.done()) {
        state_ = ProcessState::DONE;
    } else {
        state_ = ProcessState::WAITING;
    }
}

/** @brief Cancel the process and invalidate all outstanding waits. */
void Process::cancel() {
    state_ = ProcessState::CANCELLED;
    wait_group_.cancel_all();
}

void Process::restart(Task&& new_task) {
    state_ = ProcessState::CANCELLED;
    wait_group_.cancel_all();
    task_ = std::move(new_task);
    id_.generation++;
    state_ = ProcessState::ACTIVE;
}

} // namespace corosim
