/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        process.hpp
 * @brief       Coroutine process lifecycle wrapper
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Owns a Task and WaitGroup; tracks ACTIVE/WAITING/DONE/CANCELLED
 *              state.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <memory>
#include "core/types.hpp"
#include "process/task.hpp"
#include "trigger/wait.hpp"

namespace corosim {

enum class ProcessState { ACTIVE, WAITING, DONE, CANCELLED };

/** @brief One scheduled coroutine process with its wait set. */
class Process {
public:
    Process(ProcessId id, Task&& task)
        : id_(id), task_(std::move(task)), wait_group_(id) {}

    ~Process();

    Process(const Process&) = delete;
    Process& operator=(const Process&) = delete;

    ProcessId id() const { return id_; }
    ProcessId next_generation() const { return {id_.id, id_.generation + 1}; }
    bool done() const { return task_.done(); }
    bool active() const { return state_ != ProcessState::DONE && state_ != ProcessState::CANCELLED; }
    bool repeats() const { return repeating_; }
    void set_repeating(bool v) { repeating_ = v; }
    ProcessState state() const { return state_; }

    WaitGroup& waits() { return wait_group_; }
    std::coroutine_handle<> void_handle() const { return task_.void_handle(); }

    /**
     * @brief Keep the coroutine factory closure alive for the process lifetime.
     *
     * A coroutine created from a [&] lambda captures its references into the
     * factory closure object; the closure must outlive the coroutine frame.
     */
    void set_factory_holder(std::shared_ptr<void> h) { factory_holder_ = std::move(h); }

    void resume();
    void cancel();
    void restart(Task&& new_task);

    std::exception_ptr get_exception() const { return task_.get_exception(); }

private:
    ProcessId id_;
    std::shared_ptr<void> factory_holder_;
    Task task_;
    ProcessState state_ = ProcessState::ACTIVE;
    WaitGroup wait_group_;
    bool repeating_ = false;
};

} // namespace corosim
