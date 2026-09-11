#pragma once

#include <coroutine>
#include <memory>

#include "core/types.hpp"
#include "coroutine/task.hpp"
#include "coroutine/wait.hpp"

namespace corosim {

struct CoroutineId {
    uint64_t id;

    bool operator==(const CoroutineId&) const = default;
};

enum class CoroutineState { ACTIVE, WAITING, DONE, CANCELLED };

/** @brief One scheduled coroutine with its wait set. */
class Coroutine {
public:
    Coroutine(CoroutineId id, Task&& task)
        : id_(id), task_(std::move(task)) {}

    ~Coroutine() { wait_group_.cancel_all(); }

    Coroutine(const Coroutine&) = delete;
    Coroutine& operator=(const Coroutine&) = delete;

    CoroutineId id() const { return id_; }
    bool done() const { return task_.done(); }
    bool active() const { return state_ != CoroutineState::DONE && state_ != CoroutineState::CANCELLED; }
    CoroutineState state() const { return state_; }

    WaitGroup& waits() { return wait_group_; }
    std::coroutine_handle<> void_handle() const { return task_.void_handle(); }

    /**
     * @brief Keep the coroutine factory closure alive for the coroutine lifetime.
     */
    void set_factory_holder(std::shared_ptr<void> h) { factory_holder_ = std::move(h); }

    void resume();
    void cancel();

    std::exception_ptr get_exception() const { return task_.get_exception(); }

private:
    CoroutineId id_;
    std::shared_ptr<void> factory_holder_;
    Task task_;
    CoroutineState state_ = CoroutineState::ACTIVE;
    WaitGroup wait_group_;
};

} // namespace corosim
