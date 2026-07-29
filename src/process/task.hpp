/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        task.hpp
 * @brief       C++20 coroutine Task return type
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Carries Kernel/Process pointers in promise_type for awaiter
 *              registration.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <exception>
#include <functional>

namespace corosim {

class Kernel;
class Process;

/**
 * @brief Movable coroutine handle wrapper used as process body return type.
 *
 * Supports nested co_await Task via continuation in promise_type.
 */
class Task {
public:
    struct promise_type {
        Kernel* kernel = nullptr;
        Process* process = nullptr;
        std::coroutine_handle<> continuation;
        std::exception_ptr exception;

        Task get_return_object() noexcept {
            return Task(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        struct FinalAwaiter {
            bool await_ready() noexcept { return false; }
            void await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                auto& p = h.promise();
                if (p.continuation) p.continuation.resume();
            }
            void await_resume() noexcept {}
        };

        std::suspend_always initial_suspend() noexcept { return {}; }
        FinalAwaiter final_suspend() noexcept { return {}; }
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
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    ~Task() { if (handle_) handle_.destroy(); }

    bool await_ready() const noexcept { return done(); }
    void await_suspend(std::coroutine_handle<> h) noexcept {
        handle_.promise().continuation = h;
        if (handle_ && !handle_.done()) handle_.resume();
    }
    void await_resume() noexcept {}

    void resume() { if (handle_ && !handle_.done()) handle_.resume(); }
    bool done() const { return !handle_ || handle_.done(); }
    std::coroutine_handle<> void_handle() const { return handle_; }
    std::exception_ptr get_exception() const { return handle_ ? handle_.promise().exception : nullptr; }

private:
    std::coroutine_handle<promise_type> handle_;
};

} // namespace corosim
