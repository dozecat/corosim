#include "kernel.hpp"
#include "../process/process.hpp"

#include <verilated.h>

namespace corosim {

static thread_local Kernel* tls_kernel = nullptr;

namespace detail {
Kernel* current_kernel() { return tls_kernel; }
void set_current_kernel(Kernel* k) { tls_kernel = k; }

void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h) {
    auto* k = tls_kernel;
    if (!k) return;
    k->register_edge_wait(sig, edge, h);
}

void register_delay_wait(std::coroutine_handle<> h, sim_time interval) {
    auto* k = tls_kernel;
    if (!k) return;
    k->register_delay_wait(h, interval);
}

void register_compound_wait(std::coroutine_handle<> h, const TriggerInfo* infos, size_t count) {
    auto* k = tls_kernel;
    if (!k) return;
    for (size_t i = 0; i < count; ++i) {
        switch (infos[i].type) {
        case TriggerInfo::POSEDGE:
        case TriggerInfo::NEGEDGE:
        case TriggerInfo::CHANGE:
            k->register_edge_wait(infos[i].sig, infos[i].type, h);
            break;
        case TriggerInfo::DELAY:
            k->register_delay_wait(h, infos[i].interval);
            break;
        }
    }
}

} // namespace detail

Kernel::Kernel()
    : sched_(signals_) {
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

void Kernel::register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h) {
    auto* proc = proc_mgr_.find_process(h);
    if (!proc) {
        sched_.schedule_monitor(sig, edge, h, WaitId{});
        return;
    }
    auto wid = proc->waits().add_edge_watch(sig, edge);
    sched_.schedule_monitor(sig, edge, h, wid);
}

void Kernel::register_delay_wait(std::coroutine_handle<> h, sim_time interval) {
    auto* proc = proc_mgr_.find_process(h);
    auto deadline = sched_.now() + interval;
    TimerId tid = sched_.schedule_timer(deadline, h);
    if (proc) {
        proc->waits().add_delay_watch(tid);
    }
}

void Kernel::run(sim_time duration) {
    detail::set_current_kernel(this);
    proc_mgr_.init_all();
    sched_.run(duration);
    proc_mgr_.cleanup_finished();
}

} // namespace corosim
