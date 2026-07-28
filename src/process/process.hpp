#pragma once

#include <coroutine>
#include "core/types.hpp"
#include "process/task.hpp"
#include "trigger/wait.hpp"

namespace corosim {

enum class ProcessState { ACTIVE, WAITING, DONE, CANCELLED };

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

    void resume();
    void cancel();

    std::exception_ptr get_exception() const { return task_.get_exception(); }

private:
    ProcessId id_;
    Task task_;
    ProcessState state_ = ProcessState::ACTIVE;
    WaitGroup wait_group_;
    bool repeating_ = false;
};

} // namespace corosim
