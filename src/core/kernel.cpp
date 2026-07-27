#include "kernel.hpp"
#include "../process/process.hpp"

#include <verilated.h>
#include <cassert>

namespace corosim {

static thread_local Kernel* tls_kernel = nullptr;

namespace detail {
Kernel* current_kernel() { return tls_kernel; }
void set_current_kernel(Kernel* k) { tls_kernel = k; }

void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx, int* fired) {
    auto* k = tls_kernel;
    if (!k) return;
    k->register_edge_wait(sig, edge, h, fire_idx, fired);
}

void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx, int* fired) {
    auto* k = tls_kernel;
    if (!k) return;
    k->register_delay_wait(h, interval, fire_idx, fired);
}

void register_compound_wait(std::coroutine_handle<> h, const TriggerInfo* infos, size_t count, int* fired) {
    auto* k = tls_kernel;
    if (!k) return;
    for (size_t i = 0; i < count; ++i) {
        switch (infos[i].type) {
        case TriggerInfo::POSEDGE:
        case TriggerInfo::NEGEDGE:
        case TriggerInfo::CHANGE:
            k->register_edge_wait(infos[i].sig, infos[i].type, h, (int)i, fired);
            break;
        case TriggerInfo::DELAY:
            k->register_delay_wait(h, infos[i].interval, (int)i, fired);
            break;
        }
    }
}

} // namespace detail

Kernel::Kernel()
    : sched_(signals_) {
    sched_.set_proc_mgr(&proc_mgr_);
    detail::set_current_kernel(this);
}

Kernel::~Kernel() {
    detail::set_current_kernel(nullptr);
}

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
    detail::set_current_kernel(this);
    proc_mgr_.init_all();
    sched_.run(duration);

    auto ep = proc_mgr_.collect_exceptions();
    proc_mgr_.cleanup_finished();

    if (ep) std::rethrow_exception(ep);
}

} // namespace corosim
