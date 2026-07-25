/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        task.hpp
 * @brief       Task coroutine type for Verilator co-simulation
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Defines the coroutine return type used by all simulation
 *              processes. Supports co_await for coroutine composition.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/25  Initial release
 ******************************************************************************/

#pragma once

#include <coroutine>
#include <exception>

namespace corosim {

class Task {
public:
    struct promise_type {
        std::coroutine_handle<> continuation;

        Task get_return_object() noexcept {
            return Task(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        struct FinalAwaiter {
            bool await_ready() noexcept { return false; }
            void await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                auto& p = h.promise();
                if (p.continuation)
                    p.continuation.resume();
            }
            void await_resume() noexcept {}
        };

        std::suspend_always initial_suspend() noexcept { return {}; }
        FinalAwaiter final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    Task(std::coroutine_handle<promise_type> h) noexcept : handle_(h) {}

    Task(Task&& other) noexcept : handle_(other.handle_) {
        other.handle_ = nullptr;
    }

    Task& operator=(Task&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    ~Task() {
        if (handle_) handle_.destroy();
    }

    void resume() {
        if (handle_ && !handle_.done())
            handle_.resume();
    }

    bool done() const {
        return !handle_ || handle_.done();
    }

    // awaiter interface (for co_await Task)
    bool await_ready() const noexcept { return done(); }

    void await_suspend(std::coroutine_handle<> h) noexcept {
        handle_.promise().continuation = h;
        if (handle_ && !handle_.done())
            handle_.resume();
    }

    void await_resume() noexcept {}

private:
    std::coroutine_handle<promise_type> handle_;
};

} // namespace corosim
