#pragma once

#include <coroutine>
#include <exception>

namespace corosim {

class Kernel;
class Coroutine;

/**
 * @brief Movable coroutine handle wrapper used as a top-level process.
 *
 * Task is not awaitable. Spawned processes use sim.instance() and suspend only
 * on simulation events. Carries Kernel/Coroutine pointers for wait registration.
 */
class Task {
public:
    struct promise_type {
        Kernel* kernel = nullptr;
        Coroutine* coroutine = nullptr;
        std::exception_ptr exception;

        Task get_return_object() noexcept {
            return Task(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { exception = std::current_exception(); }

        static promise_type& from_handle(std::coroutine_handle<> h) {
            return std::coroutine_handle<promise_type>::from_address(h.address()).promise();
        }
    };

    Task(std::coroutine_handle<promise_type> h) noexcept : handle_(h) {}
    Task(Task&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) {
                handle_.destroy();
            }
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    ~Task() {
        if (handle_) {
            handle_.destroy();
        }
    }

    void resume() {
        if (handle_ && !handle_.done()) {
            handle_.resume();
        }
    }
    bool done() const { return !handle_ || handle_.done(); }
    std::coroutine_handle<> void_handle() const { return handle_; }
    std::exception_ptr get_exception() const { return handle_ ? handle_.promise().exception : nullptr; }

private:
    std::coroutine_handle<promise_type> handle_;
};

} // namespace corosim
