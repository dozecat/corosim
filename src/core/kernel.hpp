#pragma once

#include <coroutine>
#include <functional>
#include <memory>

#include "types.hpp"
#include "../signal/signal_registry.hpp"
#include "../scheduler/scheduler.hpp"
#include "../process/process_manager.hpp"
#include "../process/proc.hpp"

namespace corosim {

class Kernel;

namespace detail {
Kernel* current_kernel();
void set_current_kernel(Kernel* k);
}

class Kernel {
public:
    Kernel();
    ~Kernel();

    SignalRegistry& signals() { return signals_; }
    Scheduler& sched() { return sched_; }
    ProcessManager& proc_mgr() { return proc_mgr_; }

    template <typename TOP>
    void init(TOP* top, std::function<void(sim_time)> dump_fn = nullptr) {
        dump_fn_ = std::move(dump_fn);
        top_ = top;
        sched_.set_eval_fn([top] { top->eval(); });
        sched_.set_dump_fn([this](sim_time t) {
            if (top_) set_verilator_time(t);
            if (dump_fn_) dump_fn_(t);
        });
    }

    void set_verilator_time(sim_time t);

    sim_time now() const { return sched_.now(); }

    template <typename Fn> void on_tick(Fn&& fn) { sched_.on_tick(std::forward<Fn>(fn)); }
    template <typename Fn> void on_pre_eval(Fn&& fn) { sched_.on_pre_eval(std::forward<Fn>(fn)); }
    template <typename Fn> void on_post_eval(Fn&& fn) { sched_.on_post_eval(std::forward<Fn>(fn)); }
    template <typename Fn> void on_commit_eval(Fn&& fn) { sched_.on_commit_eval(std::forward<Fn>(fn)); }
    template <typename Fn> void on_comb(Fn&& fn) { sched_.on_comb(std::forward<Fn>(fn)); }
    template <typename Fn> void on_tick_end(Fn&& fn) { sched_.on_tick_end(std::forward<Fn>(fn)); }
    bool had_edge(SignalBase* sig, TriggerType edge) const { return sched_.had_edge(sig, edge); }

    template <typename Fn>
    Process* add_process(Fn&& fn) {
        return proc_mgr_.add(std::forward<Fn>(fn));
    }

    void run(sim_time duration);

    void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h);
    void register_delay_wait(std::coroutine_handle<> h, sim_time interval);

private:
    SignalRegistry signals_;
    Scheduler sched_;
    ProcessManager proc_mgr_;

    void* top_ = nullptr;
    std::function<void(sim_time)> dump_fn_;
};

} // namespace corosim
