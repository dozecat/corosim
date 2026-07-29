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
#include "core/detail/context.hpp"

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
    Process* add(Fn&& fn) {
        auto task = std::forward<Fn>(fn)();
        auto id = ProcessId{next_pid_++, 0};
        auto proc = std::make_unique<Process>(id, std::move(task));
        auto* raw = proc.get();
        processes_[id.id] = std::move(proc);

        auto& promise = Task::promise_type::from_handle(raw->void_handle());
        promise.kernel = kernel_;
        promise.process = raw;

        raw->resume();
        return raw;
    }

    Process* find_process(std::coroutine_handle<> h) const;

    size_t active_count() const;
    void cleanup_finished();
    std::exception_ptr collect_exceptions();

private:
    uint64_t next_pid_ = 1;
    Kernel* kernel_ = nullptr;
    std::unordered_map<uint64_t, std::unique_ptr<Process>> processes_;
};

} // namespace corosim
