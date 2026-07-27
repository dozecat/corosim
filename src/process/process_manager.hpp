#pragma once

#include <coroutine>
#include <functional>
#include <memory>
#include <unordered_map>

#include "proc.hpp"
#include "process.hpp"

namespace corosim {

class ProcessManager {
public:
    template <typename Fn>
    Process* add(Fn&& fn) {
        auto task = std::forward<Fn>(fn)();
        auto id = ProcessId{next_pid_++, 0};
        auto proc = std::make_unique<Process>(id, std::move(task));
        auto* raw = proc.get();
        processes_[id.id] = std::move(proc);
        // Register handle BEFORE first resume
        handle_map_[raw->void_handle().address()] = raw;
        raw->resume();
        if (!raw->done()) {
            handle_map_[raw->void_handle().address()] = raw;
        }
        return raw;
    }

    void init_all();
    Process* find_process(std::coroutine_handle<> h) const;
    size_t active_count() const;
    void cleanup_finished();

private:
    uint64_t next_pid_ = 1;
    std::unordered_map<void*, Process*> handle_map_;
    std::unordered_map<uint64_t, std::unique_ptr<Process>> processes_;
};

} // namespace corosim
