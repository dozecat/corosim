#pragma once

#include <functional>
#include <vector>
#include <queue>
#include <verilated.h>

#include "types.hpp"
#include "proc.hpp"

namespace corosim {

class Engine {
public:
    Engine();
    ~Engine();
    static Engine& current();

    // ---- simulation control ----
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

    // ---- lifecycle hooks ----
    using hook_fn = std::function<void()>;
    void on_tick(hook_fn);         // Phase 1: each time step starts
    void on_commit(hook_fn);       // Phase 1 commit after tick
    void on_pre_eval(hook_fn);     // Phase 2: before eval
    void on_post_eval(hook_fn);    // Phase 4: after eval
    void on_commit_eval(hook_fn);  // Phase 4 commit after eval/drive
    void on_comb(hook_fn);         // Phase 5: delta loop
    void on_tick_end(hook_fn);     // Phase 7: before edge clear

    // ---- coroutine registration ----
    void add_proc(std::function<Proc()> fn);

    // ---- coroutine scheduler ----
    void watch_edge(void* sig, TriggerInfo::Type edge, std::coroutine_handle<> h);
    void watch_delay(std::coroutine_handle<> h, sim_time t);

    // ---- signal query ----
    bool had_edge(void* sig, TriggerInfo::Type edge);

private:
    // internal methods
    void commit_all();
    void fire_hooks(std::vector<hook_fn>& hooks);
    void check_edge_watchers();
    void process_delay_wakeups();

    // hook lists
    std::vector<hook_fn> tick_;
    std::vector<hook_fn> commit_;
    std::vector<hook_fn> pre_eval_;
    std::vector<hook_fn> post_eval_;
    std::vector<hook_fn> commit_eval_;
    std::vector<hook_fn> comb_;
    std::vector<hook_fn> tick_end_;

    // coroutine processes
    struct CoroProc { Proc task; };
    std::vector<CoroProc> coro_procs_;

    // edge watchers (for co_await)
    struct EdgeWatcher {
        void* sig;
        TriggerInfo::Type edge;
        std::coroutine_handle<> handle;
    };
    std::vector<EdgeWatcher> edge_watchers_;

    // delay wakeups (for co_await)
    struct WakeupEvent {
        sim_time time;
        std::coroutine_handle<> handle;
        bool operator>(const WakeupEvent& o) const { return time > o.time; }
    };
    std::priority_queue<WakeupEvent, std::vector<WakeupEvent>, std::greater<>> wakeup_queue_;

    // simulation state
    sim_time now_ = 0;
    void* top_ = nullptr;
    std::function<void()> eval_fn_;
    std::function<void(sim_time)> dump_fn_;
};

} // namespace corosim
