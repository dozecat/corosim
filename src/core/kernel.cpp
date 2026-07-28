#include "kernel.hpp"
#include "process/process.hpp"

#include <verilated.h>
#include <cassert>

namespace corosim {

Kernel::Kernel()
    : sched_(signals_) {
    proc_mgr_.set_kernel(this);
    sched_.set_cancel_fn([this](auto h, auto wid) {
        auto* proc = proc_mgr_.find_process(h);
        if (proc) proc->waits().cancel_others(wid);
    });
}

Kernel::~Kernel() {}

void Kernel::set_verilator_time(sim_time t) {
    if (top_) {
        auto* ctp = static_cast<VerilatedModel*>(top_)->contextp();
        if (ctp) ctp->time(t);
    }
}

void Kernel::register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx, int* fired) {
    auto* proc = proc_mgr_.find_process(h);
    assert(proc && "coroutine handle not found in ProcessManager");
    auto* wid = proc->waits().add_edge_watch(sig, edge);
    sched_.schedule_monitor(sig, edge, h, wid, fire_idx, fired);
}

void Kernel::register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx, int* fired) {
    auto* proc = proc_mgr_.find_process(h);
    assert(proc && "coroutine handle not found in ProcessManager");
    auto* wid = proc->waits().add_delay_watch(TimerId{0});
    auto deadline = sched_.now() + interval;
    TimerId tid = sched_.schedule_timer(deadline, h, wid, fire_idx, fired);
    wid->target = tid;
    (void)tid;
}

void Kernel::run(sim_time duration) {
    proc_mgr_.init_all();
    sched_.run(duration);

    auto ep = proc_mgr_.collect_exceptions();
    proc_mgr_.cleanup_finished();

    if (ep) std::rethrow_exception(ep);
}

} // namespace corosim
