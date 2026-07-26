#include "engine.hpp"
#include "signal.hpp"
#include <cstdio>

namespace corosim {

static thread_local Engine* current_engine = nullptr;

// ---- detail proxy functions ----
namespace detail {

void register_edge_watcher(void* sig_base, TriggerInfo::Type edge, std::coroutine_handle<> h) {
    if (current_engine) current_engine->watch_edge(sig_base, edge, h);
}

void register_delay_wakeup(std::coroutine_handle<> h, sim_time interval) {
    if (current_engine) current_engine->watch_delay(h, current_engine->now() + interval);
}

void register_proc(std::function<Proc()> fn) {
    if (current_engine) current_engine->add_proc(std::move(fn));
}

} // namespace detail

// ---- Engine ----
Engine::Engine() {
    if (current_engine) { std::fprintf(stderr, "FATAL: only one Engine instance\n"); std::abort(); }
    current_engine = this;
}

Engine::~Engine() { current_engine = nullptr; }

Engine& Engine::current() {
    if (!current_engine) { std::fprintf(stderr, "FATAL: Engine::current() before construction\n"); std::abort(); }
    return *current_engine;
}

// ---- hooks ----
void Engine::on_tick(hook_fn fn) { tick_.push_back(std::move(fn)); }
void Engine::on_commit(hook_fn fn) { commit_.push_back(std::move(fn)); }
void Engine::on_pre_eval(hook_fn fn) { pre_eval_.push_back(std::move(fn)); }
void Engine::on_post_eval(hook_fn fn) { post_eval_.push_back(std::move(fn)); }
void Engine::on_commit_eval(hook_fn fn) { commit_eval_.push_back(std::move(fn)); }
void Engine::on_comb(hook_fn fn) { comb_.push_back(std::move(fn)); }
void Engine::on_tick_end(hook_fn fn) { tick_end_.push_back(std::move(fn)); }

// ---- coroutines ----
void Engine::add_proc(std::function<Proc()> fn) {
    auto p = fn();
    coro_procs_.push_back({std::move(p)});
}

// ---- scheduler ----
void Engine::watch_edge(void* sig, TriggerInfo::Type edge, std::coroutine_handle<> h) {
    edge_watchers_.push_back({sig, edge, h});
}

void Engine::watch_delay(std::coroutine_handle<> h, sim_time t) {
    wakeup_queue_.push({t, h});
}

// ---- signal query ----
bool Engine::had_edge(void* sig, TriggerInfo::Type edge) {
    auto* base = static_cast<detail::SignalBase*>(sig);
    bool r = false;
    switch (edge) {
    case TriggerInfo::POSEDGE: r = base->had_posedge(); break;
    case TriggerInfo::NEGEDGE: r = base->had_negedge(); break;
    case TriggerInfo::CHANGE:  r = base->is_dirty(); break;
    default: break;
    }
    return r;
}

// ---- internal ----
void Engine::fire_hooks(std::vector<hook_fn>& hooks) {
    for (auto& h : hooks) if (h) h();
}

void Engine::commit_all() {
    for (auto* sig : SignalRegistry::all()) {
        if (sig) sig->commit();
    }
}

void Engine::check_edge_watchers() {
    auto watchers = std::move(edge_watchers_);
    edge_watchers_.clear();

    for (auto& w : watchers) {
        if (w.handle.done()) continue;
        bool triggered = false;
        switch (w.edge) {
        case TriggerInfo::POSEDGE:
        case TriggerInfo::NEGEDGE:
            triggered = had_edge(w.sig, w.edge);
            break;
        case TriggerInfo::CHANGE:  {
            auto* base = static_cast<detail::SignalBase*>(w.sig);
            triggered = base->is_dirty();
            break;
        }
        default: break;
        }
        if (triggered) w.handle.resume();
        else edge_watchers_.push_back(w);
    }
}

void Engine::process_delay_wakeups() {
    while (!wakeup_queue_.empty() && wakeup_queue_.top().time <= now_) {
        auto h = wakeup_queue_.top().handle;
        wakeup_queue_.pop();
        if (!h.done()) h.resume();
    }
}

// ---- run ----
void Engine::run(sim_time duration) {
    current_engine = this;

    // --- init ---
    fire_hooks(tick_);
    commit_all();
    fire_hooks(commit_);

    fire_hooks(pre_eval_);
    if (eval_fn_) eval_fn_();
    fire_hooks(post_eval_);
    commit_all();
    fire_hooks(commit_eval_);

    set_time(0);
    if (dump_fn_) dump_fn_(0);

    for (auto& c : coro_procs_) c.task.resume();
    for (auto* sig : SignalRegistry::all()) sig->clear_edge_flags();

    // --- main loop ---
    for (now_ = 1; now_ <= duration; now_++) {
        // Phase 1: tick → commit
        fire_hooks(tick_);
        commit_all();
        fire_hooks(commit_);

        // Phase 2: pre_eval
        fire_hooks(pre_eval_);

        // Phase 3: eval
        if (eval_fn_) eval_fn_();

        // Phase 4: post_eval → commit
        fire_hooks(post_eval_);
        commit_all();
        fire_hooks(commit_eval_);

        // Phase 5: comb delta
        for (int delta = 0; delta < 100; delta++) {
            fire_hooks(comb_);
            commit_all();
            bool dirty = false;
            for (auto* sig : SignalRegistry::all()) {
                if (sig->is_dirty()) { dirty = true; sig->clear_dirty(); }
            }
            if (!dirty) break;
        }

        // Phase 6: coroutine watchers
        check_edge_watchers();
        process_delay_wakeups();

        // Phase 7: tick_end → clear edge
        fire_hooks(tick_end_);
        for (auto* sig : SignalRegistry::all()) sig->clear_edge_flags();

        // Phase 8: dump
        set_time(now_);
        if (dump_fn_) dump_fn_(now_);
    }
}

} // namespace corosim
