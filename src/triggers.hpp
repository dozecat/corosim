/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        triggers.hpp
 * @brief       Extended trigger primitives: clock_cycles, Event, first
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Provides higher-level coroutine synchronization primitives:
 *              clock_cycles — wait for N clock edges
 *              Event        — manual fire-and-wait event
 *              first        — wait for the first of multiple events
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/25  Initial release
 ******************************************************************************/

#pragma once

#include <memory>
#include <functional>
#include "task.hpp"
#include "signal.hpp"
#include "delay.hpp"

namespace corosim {

template <typename T>
Task clock_cycles(Signal<T>& clk, int n) {
    for (int i = 0; i < n; i++)
        co_await posedge(clk);
}

class Event {
    bool fired_ = false;
    std::coroutine_handle<> waiter_;
    std::function<void()> cb_;

public:
    struct Awaiter {
        Event* self;
        bool await_ready() noexcept { return self->fired_; }
        void await_suspend(std::coroutine_handle<> h) noexcept { self->waiter_ = h; }
        void await_resume() noexcept { self->fired_ = false; }
    };

    Awaiter wait() { return Awaiter{this}; }

    void set() {
        if (cb_) {
            auto cb = std::move(cb_);
            cb();
        } else if (waiter_) {
            fired_ = true;
            auto h = waiter_;
            waiter_ = nullptr;
            h.resume();
        } else {
            fired_ = true;
        }
    }

    void set_callback(std::function<void()> cb) {
        cb_ = std::move(cb);
        if (fired_) {
            fired_ = false;
            auto cb = std::move(cb_);
            cb();
        }
    }

    void clear_callback() { cb_ = nullptr; }
};

namespace detail {

struct FirstShared {
    int* winner;
    std::coroutine_handle<> handle;
};

struct FirstAwaiter {
    std::shared_ptr<FirstShared> shared;
    bool await_ready() noexcept { return *shared->winner >= 0; }
    void await_suspend(std::coroutine_handle<> h) noexcept { shared->handle = h; }
    void await_resume() noexcept {}
};

} // namespace detail

template <typename... Events>
Task first(int& winner, Events&... events) {
    auto shared = std::make_shared<detail::FirstShared>();
    shared->winner = &winner;
    winner = -1;

    int idx = 0;
    ([&]{
        events.set_callback([shared, i = idx] {
            if (*shared->winner < 0) {
                *shared->winner = i;
                if (shared->handle)
                    shared->handle.resume();
            }
        });
        idx++;
    }(), ...);

    co_await detail::FirstAwaiter{shared};

    (events.clear_callback(), ...);

    // keep winner available even though we used co_return void
}

} // namespace corosim
