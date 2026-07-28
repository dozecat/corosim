#pragma once

#include <coroutine>
#include <functional>
#include <memory>
#include <cassert>

#include "core/types.hpp"
#include "signal/signal_registry.hpp"
#include "scheduler/scheduler.hpp"
#include "process/process_manager.hpp"
#include "process/task.hpp"
#include "trigger/edge.hpp"
#include "trigger/delay.hpp"

namespace corosim {

class Kernel {
public:
    Kernel();
    ~Kernel();

    SignalRegistry& signals() { return signals_; }
    Scheduler& sched() { return sched_; }
    ProcessManager& proc_mgr() { return proc_mgr_; }

    template <typename TOP>
    void init(TOP* top, std::function<void(sim_time)> dump_fn = nullptr);

    void set_verilator_time(sim_time t);

    sim_time now() const { return sched_.now(); }

    template <typename Fn> void on_pre_eval(Fn&& fn);
    template <typename Fn> void on_post_eval(Fn&& fn);
    template <typename Fn> void on_comb(Fn&& fn);
    bool had_edge(SignalBase* sig, TriggerType edge) const { return sched_.had_edge(sig, edge); }

    template <typename Fn>
    Process* add_process(Fn&& fn);

    // ── API ──

    template <typename Trigger, typename Fn>
    void always(Trigger t, Fn fn);

    template <typename Fn>
    void always_comb(Fn fn);

    template <typename Fn, typename... Args>
    void instance(Fn&& fn, Args&&... args);

    template <typename Trigger, typename Fn>
    void sample(Trigger t, Fn fn);

    template <typename Trigger, typename Fn>
    void drive(Trigger t, Fn fn);

    template <typename Trigger, typename Device>
    void device(Trigger t, Device& dev);

    void run(sim_time duration);

    void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx = -1, int* fired = nullptr);
    void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx = -1, int* fired = nullptr);

private:
    SignalRegistry signals_;
    Scheduler sched_;
    ProcessManager proc_mgr_;

    void* top_ = nullptr;
    std::function<void(sim_time)> dump_fn_;
};

} // namespace corosim
