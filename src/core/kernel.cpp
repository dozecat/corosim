#include "core/kernel.hpp"

#include <cassert>

#include <verilated.h>

#include "coroutine/coroutine.hpp"

namespace corosim {

namespace {
/** @brief Resolve owning Coroutine from a coroutine handle's promise. */
Coroutine* get_coroutine_from_handle(std::coroutine_handle<> h) {
    return Task::promise_type::from_handle(h).coroutine;
}
} // namespace

Kernel::Kernel()
    : sched_(signals_) {
    // any(): when one wait fires, invalidate sibling WaitTokens on the same coroutine.
    sched_.set_cancel_fn([](std::coroutine_handle<> h, const std::shared_ptr<WaitToken>& keep) {
        auto* c = get_coroutine_from_handle(h);
        if (c) c->waits().cancel_others(keep);
    });
    // Signal destruction notifies scheduler to clean monitor entries.
    signals_.on_signal_destroyed = [this](SignalVal* s) { sched_.on_signal_destroy(s); };
}

Kernel::~Kernel() {}

/** @brief Push simulation time into the Verilator context for VCD dumps. */
void Kernel::set_verilator_time(sim_time t) {
    if (top_) {
        auto* ctp = static_cast<VerilatedModel*>(top_)->contextp();
        if (ctp) ctp->time(t);
    }
}

void Kernel::register_trigger_wait(SignalVal* sig, TriggerType t, std::coroutine_handle<> h,
                                   int fire_idx, int* fired) {
    auto* c = get_coroutine_from_handle(h);
    assert(c && "coroutine handle not found in CoroutineManager");
    auto token = c->waits().add_trigger_watch(sig, t);
    sched_.schedule_monitor(sig, t, h, token, fire_idx, fired);
}

void Kernel::register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx, int* fired) {
    auto* c = get_coroutine_from_handle(h);
    assert(c && "coroutine handle not found in CoroutineManager");
    auto token = c->waits().add_delay_watch();
    sched_.schedule_timer(sched_.now() + interval, h, token, fire_idx, fired);
}

/** @brief Run the scheduler, then rethrow the first coroutine exception if any. */
void Kernel::run(sim_time duration) {
    sched_.run(duration);

    auto ep = coroutine_manager_.collect_exceptions();
    coroutine_manager_.cleanup_finished();

    if (ep) std::rethrow_exception(ep);
}

} // namespace corosim
