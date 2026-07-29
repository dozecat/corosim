/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        process_manager.cpp
 * @brief       ProcessManager method implementations
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Lookup, active counting, cleanup, and exception collection.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#include "process_manager.hpp"
#include "process.hpp"

namespace corosim {

Process* ProcessManager::find_process(std::coroutine_handle<> h) const {
    return Task::promise_type::from_handle(h).process;
}

size_t ProcessManager::active_count() const {
    size_t n = 0;
    for (auto& [id, proc] : processes_) {
        if (proc && proc->active() && !proc->done()) n++;
    }
    return n;
}

/** @brief Erase finished or null process entries. */
void ProcessManager::cleanup_finished() {
    auto it = processes_.begin();
    while (it != processes_.end()) {
        if (!it->second || it->second->done()) {
            it = processes_.erase(it);
        } else {
            ++it;
        }
    }
}

/** @brief Return the first exception from a finished process, if any. */
std::exception_ptr ProcessManager::collect_exceptions() {
    for (auto& [id, proc] : processes_) {
        if (proc && proc->done()) {
            auto ep = proc->get_exception();
            if (ep) return ep;
        }
    }
    return nullptr;
}

} // namespace corosim
