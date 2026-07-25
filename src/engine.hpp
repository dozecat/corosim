/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        engine.hpp
 * @brief       Simulation engine declaration
 * @see         https://github.com/dozecat/corosim
 *
 * @details     The Engine class manages the Verilator simulation loop.
 *              pre_eval/post_eval conventions follow SimCoop/vaxivip:
 *              pre_eval  → BFM update_input (before eval, sample DUT outputs)
 *              post_eval → BFM update_output (after eval, drive DUT inputs)
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/25  Initial release
 ******************************************************************************/

#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <verilated.h>

#include "task.hpp"
#include "delay.hpp"
#include "signal.hpp"

namespace corosim {

class Engine {
public:
    Engine();
    ~Engine();

    void always(Delay d, std::function<void()> fn);
    template <typename T> void always(Posedge<T> p, std::function<void()> fn);
    template <typename T> void always(Negedge<T> n, std::function<void()> fn);
    template <typename T> void always(Change<T> c, std::function<void()> fn);
    void always_comb(std::function<void()> fn);

    void task(std::function<Task()> fn);

    void pre_eval(std::function<void()> fn);
    void post_eval(std::function<void()> fn);

    template <typename TOP>
    void init(TOP* top, std::function<void(sim_time)> dump_fn = nullptr) {
        dump_fn_ = std::move(dump_fn);
        top_ = top;
        eval_fn_ = [top] { top->eval(); };
    }

    void set_time(sim_time t) {
        if (top_) {
            auto* ctp = static_cast<VerilatedModel*>(top_)->contextp();
            if (ctp) ctp->time(t);
        }
    }

    void run(sim_time duration);
    sim_time now() const { return now_; }

    static Engine& current();
    void schedule_wakeup(std::coroutine_handle<> h, sim_time t);
    void add_edge_watcher(SignalBase* sig, EdgeType edge, std::coroutine_handle<> h);

private:
    struct SimpleProc {
        enum Type { DELAY, POSEDGE, NEGEDGE, CHANGE, ALWAYS_COMB };
        Type type;
        SignalBase* sig;
        sim_time interval;
        sim_time next_wakeup;
        std::function<void()> fn;
    };

    struct CoroProc {
        Task task;
    };

    struct WakeupEvent {
        sim_time time;
        std::coroutine_handle<> handle;
        bool operator>(const WakeupEvent& o) const { return time > o.time; }
    };

    struct EdgeWatcher {
        SignalBase* sig;
        EdgeType edge;
        std::coroutine_handle<> handle;
    };

    void commit_all();
    void run_always_comb();
    void check_edge_watchers();
    void process_delay_wakeups();

    sim_time now_ = 0;
    std::vector<SimpleProc> simple_procs_;
    std::vector<CoroProc> coro_procs_;
    std::vector<std::function<void()>> pre_callbacks_;
    std::vector<std::function<void()>> post_callbacks_;
    std::priority_queue<WakeupEvent, std::vector<WakeupEvent>, std::greater<>> wakeup_queue_;
    std::vector<EdgeWatcher> edge_watchers_;
    std::function<void()> eval_fn_;
    std::function<void(sim_time)> dump_fn_;
    void* top_ = nullptr;
};

template <typename T>
void Engine::always(Posedge<T> p, std::function<void()> fn) {
    simple_procs_.push_back({SimpleProc::POSEDGE,
        &p.signal(), 0, 0, std::move(fn)});
}

template <typename T>
void Engine::always(Negedge<T> n, std::function<void()> fn) {
    simple_procs_.push_back({SimpleProc::NEGEDGE,
        &n.signal(), 0, 0, std::move(fn)});
}

template <typename T>
void Engine::always(Change<T> c, std::function<void()> fn) {
    simple_procs_.push_back({SimpleProc::CHANGE,
        &c.signal(), 0, 0, std::move(fn)});
}

} // namespace corosim
