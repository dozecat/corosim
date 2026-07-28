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

class ProcessManager {
public:
    void set_kernel(Kernel* k) { kernel_ = k; }

    template <typename Fn>
    Process* add(Fn&& fn) {
        auto task = std::forward<Fn>(fn)();
        auto id = ProcessId{next_pid_++, 0};
        auto proc = std::make_unique<Process>(id, std::move(task));
        auto* raw = proc.get();
        processes_[id.id] = std::move(proc);
        handle_map_[raw->void_handle().address()] = raw;
        detail::map_handle(raw->void_handle(), kernel_);
        raw->resume();
        if (!raw->done()) {
            handle_map_[raw->void_handle().address()] = raw;
            detail::map_handle(raw->void_handle(), kernel_);
        }
        return raw;
    }

    void init_all();
    Process* find_process(std::coroutine_handle<> h) const;
    size_t active_count() const;
    void cleanup_finished();
    std::exception_ptr collect_exceptions();

private:
    uint64_t next_pid_ = 1;
    Kernel* kernel_ = nullptr;
    std::unordered_map<void*, Process*> handle_map_;
    std::unordered_map<uint64_t, std::unique_ptr<Process>> processes_;
};

} // namespace corosim
