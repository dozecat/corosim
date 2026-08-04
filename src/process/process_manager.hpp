/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        process_manager.hpp
 * @brief       Creates and tracks coroutine processes
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Allocates ProcessIds, binds promise context, and collects
 *              exceptions.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <functional>
#include <memory>
#include <unordered_map>

#include "task.hpp"
#include "process.hpp"
#include "core/detail/wait_register.hpp"

namespace corosim {

class Kernel;

/** @brief Owns Process instances and wires Task promise to Kernel. */
class ProcessManager {
public:
    void set_kernel(Kernel* k) { kernel_ = k; }

    /**
     * @brief Create a process from a factory that returns Task.
     * @param fn Invoked once; return value is the process body.
     */
    template <typename Fn>
    Process* spawn(Fn&& fn) {
        auto holder = std::make_shared<std::decay_t<Fn>>(std::forward<Fn>(fn));
        auto task = (*holder)();
        auto id = ProcessId{next_pid_++, 0};
        auto proc = std::make_unique<Process>(id, std::move(task));
        proc->set_factory_holder(std::move(holder));
        auto* raw = proc.get();
        processes_[id.id] = std::move(proc);

        auto& promise = Task::promise_type::from_handle(raw->void_handle());
        promise.kernel = kernel_;
        promise.process = raw;

        raw->resume();
        return raw;
    }

    Process* find_process(std::coroutine_handle<> h) const;

    /**
     * @brief Replace the body of process @p id with a fresh coroutine.
     * @return The process pointer (same memory, bumped generation).
     */
    template <typename Fn>
    Process* restart(ProcessId id, Fn&& fn) {
        auto it = processes_.find(id.id);
        if (it == processes_.end() || !it->second) return nullptr;
        auto* proc = it->second.get();
        auto holder = std::make_shared<std::decay_t<Fn>>(std::forward<Fn>(fn));
        auto new_task = (*holder)();
        proc->restart(std::move(new_task));
        proc->set_factory_holder(std::move(holder));
        auto& promise = Task::promise_type::from_handle(proc->void_handle());
        promise.kernel = kernel_;
        promise.process = proc;
        proc->resume();
        return proc;
    }

    size_t active_count() const;
    void cleanup_finished();
    std::exception_ptr collect_exceptions();

private:
    uint64_t next_pid_ = 1;
    Kernel* kernel_ = nullptr;
    std::unordered_map<uint64_t, std::unique_ptr<Process>> processes_;
};

} // namespace corosim
