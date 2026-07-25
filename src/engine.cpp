/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        engine.cpp
 * @brief       Simulation engine implementation
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Engine::run() implements the Verilator co-simulation loop.
 *              Pre/Post naming follows SimCoop/vaxivip convention.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/25  Initial release
 ******************************************************************************/

#include "engine.hpp"
#include <algorithm>
#include <cstdio>

namespace corosim {

static thread_local Engine* current_engine = nullptr;

namespace detail {

void register_delay_wakeup(std::coroutine_handle<> h, sim_time interval) {
    if (current_engine)
        current_engine->schedule_wakeup(h, current_engine->now() + interval);
}

void register_edge_watcher(void* sig_base, int edge, std::coroutine_handle<> h) {
    if (current_engine)
        current_engine->add_edge_watcher(
            static_cast<SignalBase*>(sig_base), static_cast<EdgeType>(edge), h);
}

} // namespace detail

Engine::Engine() {
    if (current_engine) {
        std::fprintf(stderr, "FATAL: only one Engine instance allowed\n");
        std::abort();
    }
    current_engine = this;
}

Engine::~Engine() { current_engine = nullptr; }

Engine& Engine::current() {
    if (!current_engine) {
        std::fprintf(stderr, "FATAL: Engine::current() called before Engine construction\n");
        std::abort();
    }
    return *current_engine;
}

void Engine::always(Delay d, std::function<void()> fn) {
    simple_procs_.push_back({SimpleProc::DELAY, nullptr, d.interval, 0, std::move(fn)});
}

void Engine::always_comb(std::function<void()> fn) {
    simple_procs_.push_back({SimpleProc::ALWAYS_COMB, nullptr, 0, 0, std::move(fn)});
}

void Engine::task(std::function<Task()> fn) {
    coro_procs_.push_back({fn()});
}

void Engine::pre_eval(std::function<void()> fn) {
    pre_callbacks_.push_back(std::move(fn));
}

void Engine::post_eval(std::function<void()> fn) {
    post_callbacks_.push_back(std::move(fn));
}

void Engine::schedule_wakeup(std::coroutine_handle<> h, sim_time t) {
    wakeup_queue_.push({t, h});
}

void Engine::add_edge_watcher(SignalBase* sig, EdgeType edge,
                              std::coroutine_handle<> h) {
    edge_watchers_.push_back({sig, edge, h});
}

void Engine::commit_all() {
    for (auto* sig : SignalRegistry::all()) {
        sig->commit();
    }
}

void Engine::run_always_comb() {
    constexpr int max_delta = 100;
    for (int delta = 0; delta < max_delta; delta++) {
        for (auto& p : simple_procs_) {
            if (p.type == SimpleProc::ALWAYS_COMB && p.fn) p.fn();
        }
        commit_all();

        bool changed = false;
        for (auto* sig : SignalRegistry::all()) {
            if (sig->is_dirty()) {
                changed = true;
                sig->clear_dirty();
            }
        }
        if (!changed) return;
    }
    std::fprintf(stderr, "WARNING: always_comb did not converge after %d delta cycles\n", max_delta);
}

void Engine::check_edge_watchers() {
    auto watchers = std::move(edge_watchers_);
    edge_watchers_.clear();

    for (auto& w : watchers) {
        if (w.handle.done()) continue;

        bool triggered = false;
        switch (w.edge) {
        case POSEDGE: triggered = w.sig->had_posedge(); break;
        case NEGEDGE: triggered = w.sig->had_negedge(); break;
        case CHANGE:  triggered = w.sig->is_dirty();    break;
        }

        if (triggered) {
            w.handle.resume();
        } else {
            edge_watchers_.push_back(w);
        }
    }
}

void Engine::process_delay_wakeups() {
    while (!wakeup_queue_.empty() && wakeup_queue_.top().time <= now_) {
        auto h = wakeup_queue_.top().handle;
        wakeup_queue_.pop();
        if (!h.done()) h.resume();
    }
}

void Engine::run(sim_time duration) {
    current_engine = this;

    // Init: only run DELAY (set wakeup) and ALWAYS_COMB (init values)
    // Edge/change procs fire on actual edges only
    for (auto& p : simple_procs_) {
        if (p.type == SimpleProc::DELAY)
            p.next_wakeup = now_ + p.interval;
        if ((p.type == SimpleProc::DELAY || p.type == SimpleProc::ALWAYS_COMB) && p.fn)
            p.fn();
    }
    commit_all();

    // First eval cycle with BFM hooks
    for (auto& cb : pre_callbacks_) cb();
    if (eval_fn_) eval_fn_();
    for (auto& cb : post_callbacks_) cb();
    commit_all();

    set_time(0);
    if (dump_fn_) dump_fn_(0);

    // Start coroutine tasks (resume past initial_suspend)
    for (auto& c : coro_procs_) c.task.resume();

    // Clear edge flags from init so main loop starts clean
    for (auto* sig : SignalRegistry::all())
        sig->clear_edge_flags();

    // Main simulation loop
    for (now_ = 1; now_ <= duration; now_++) {
        // Phase 1: delay-triggered processes
        for (auto& p : simple_procs_) {
            if (p.type == SimpleProc::DELAY && p.next_wakeup == now_) {
                p.fn();
                p.next_wakeup = now_ + p.interval;
            }
        }
        commit_all();

        // Phase 2: pre_eval callbacks (BFM update_input)
        for (auto& cb : pre_callbacks_) cb();

        // Phase 3: eval
        if (eval_fn_) eval_fn_();

        // Phase 4: edge/change-triggered processes
        for (auto& p : simple_procs_) {
            if (!p.fn) continue;
            switch (p.type) {
            case SimpleProc::POSEDGE:
                if (p.sig && p.sig->had_posedge()) p.fn();
                break;
            case SimpleProc::NEGEDGE:
                if (p.sig && p.sig->had_negedge()) p.fn();
                break;
            case SimpleProc::CHANGE:
                if (p.sig && p.sig->is_dirty()) p.fn();
                break;
            default: break;
            }
        }
        commit_all();

        // Phase 5: post_eval callbacks (BFM update_output)
        if (!post_callbacks_.empty() && now_ <= 5) fprintf(stderr, "PH5 t=%llu\n", (unsigned long long)now_);
        for (auto& cb : post_callbacks_) cb();
        commit_all();

        // Phase 6: always_comb
        run_always_comb();

        // Phase 7: coroutine watchers
        check_edge_watchers();
        process_delay_wakeups();

        // Phase 8: clear edge flags
        for (auto* sig : SignalRegistry::all())
            sig->clear_edge_flags();

        // Phase 9: waveform dump
        set_time(now_);
        if (dump_fn_) dump_fn_(now_);
    }
}

} // namespace corosim
