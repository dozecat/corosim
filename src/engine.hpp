/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        engine.hpp
 * @brief       Simulation engine declaration
 * @see         https://github.com/dozecat/corosim
 *
 * @details     The Engine class manages the Verilator simulation loop.
 *              Phase ordering:
 *                1  delay → commit_all
 *                2  sample_cb + pre_eval
 *                3  top->eval()
 *                4  drive_cb (= always) + post_eval → commit_all
 *                5  always_comb
 *                6  coroutine edge watchers + delay wakeups
 *                7  clear edge flags
 *                8  waveform dump
 *
 *              sample_cb  → BFM update_input (before eval, edge-triggered)
 *              drive_cb   → BFM update_output (after eval, edge-triggered)
 *              always     → same timing as drive_cb (Phase 4)
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

    // ---- process registration ----

    // always / drive_cb: Phase 4 (after eval, edge-triggered)
    void always(Delay d, std::function<void()> fn);
    template <typename T> void always(Posedge<T> p, std::function<void()> fn);
    template <typename T> void always(Negedge<T> n, std::function<void()> fn);
    template <typename T> void always(Change<T> c, std::function<void()> fn);

    template <typename T> void drive_cb(Posedge<T> p, std::function<void()> fn) { always(p, std::move(fn)); }
    template <typename T> void drive_cb(Negedge<T> p, std::function<void()> fn) { always(p, std::move(fn)); }
    template <typename T> void drive_cb(Change<T> p, std::function<void()> fn) { always(p, std::move(fn)); }

    // sample_cb: Phase 2 (before eval, edge-triggered)
    template <typename T> void sample_cb(Posedge<T> p, std::function<void()> fn);
    template <typename T> void sample_cb(Negedge<T> p, std::function<void()> fn);
    template <typename T> void sample_cb(Change<T> c, std::function<void()> fn);

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
    struct Proc {
        enum Type { DELAY, POSEDGE, NEGEDGE, CHANGE, ALWAYS_COMB };
        Type type;
        SignalBase* sig;
        sim_time interval;
        sim_time next_wakeup;
        std::function<void()> fn;
    };

    struct CoroProc { Task task; };

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
    void run_edge_procs(std::vector<Proc>& procs);
    void run_always_comb();
    void check_edge_watchers();
    void process_delay_wakeups();

    sim_time now_ = 0;
    std::vector<Proc> simple_procs_;    // Phase 4 (after eval)
    std::vector<Proc> sample_procs_;    // Phase 2 (before eval)
    std::vector<CoroProc> coro_procs_;
    std::vector<std::function<void()>> pre_callbacks_;
    std::vector<std::function<void()>> post_callbacks_;
    std::priority_queue<WakeupEvent, std::vector<WakeupEvent>, std::greater<>> wakeup_queue_;
    std::vector<EdgeWatcher> edge_watchers_;
    std::function<void()> eval_fn_;
    std::function<void(sim_time)> dump_fn_;
    void* top_ = nullptr;
};

// ---- sample_cb templates ----
template <typename T>
void Engine::sample_cb(Posedge<T> p, std::function<void()> fn) {
    sample_procs_.push_back({Proc::POSEDGE, &p.signal(), 0, 0, std::move(fn)});
}

template <typename T>
void Engine::sample_cb(Negedge<T> n, std::function<void()> fn) {
    sample_procs_.push_back({Proc::NEGEDGE, &n.signal(), 0, 0, std::move(fn)});
}

template <typename T>
void Engine::sample_cb(Change<T> c, std::function<void()> fn) {
    sample_procs_.push_back({Proc::CHANGE, &c.signal(), 0, 0, std::move(fn)});
}

// ---- always (Phase 4, after eval) ----
template <typename T>
void Engine::always(Posedge<T> p, std::function<void()> fn) {
    simple_procs_.push_back({Proc::POSEDGE, &p.signal(), 0, 0, std::move(fn)});
}

template <typename T>
void Engine::always(Negedge<T> n, std::function<void()> fn) {
    simple_procs_.push_back({Proc::NEGEDGE, &n.signal(), 0, 0, std::move(fn)});
}

template <typename T>
void Engine::always(Change<T> c, std::function<void()> fn) {
    simple_procs_.push_back({Proc::CHANGE, &c.signal(), 0, 0, std::move(fn)});
}

} // namespace corosim
