#include "scheduler.hpp"
#include <algorithm>

namespace corosim {

TimerId Scheduler::schedule_timer(sim_time deadline, std::coroutine_handle<> h, WaitId* wid,
                                  int fire_idx, int* fired) {
    return timer_.schedule(deadline, h, wid, fire_idx, fired);
}

void Scheduler::schedule_monitor(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h,
                                  WaitId* wid, int fire_idx, int* fired) {
    monitor_.watch(sig, edge, h, wid, fire_idx, fired);
}

void Scheduler::fire_coroutine(std::coroutine_handle<> h, WaitId* wid, int* fire_idx, int fire_value) {
    if (fire_idx) *fire_idx = fire_value;
    if (cancel_others_fn_) cancel_others_fn_(h, wid);
    if (wid) wid->invalidate();
    h.resume();
}

void Scheduler::run_one_tick() {
    auto fire_lambda = [this](std::coroutine_handle<> h, WaitId* wid, int* fire_idx, int fire_value) {
        fire_coroutine(h, wid, fire_idx, fire_value);
    };

    // Advance any remaining timers at the current time (e.g. scheduled by a
    // monitor callback earlier in the same tick → chained delta).
    timer_.advance_to(timer_.now(), [&](const TimerEngine::Event& ev) {
        fire_coroutine(ev.handle, ev.wid, ev.fire_idx, ev.fire_value);
    });

    delta_.eval();
    monitor_.process(signals_.changed_signals(), fire_lambda);
    delta_.commit();
    delta_.end_tick();

    if (dump_fn_) dump_fn_(timer_.now());
}

void Scheduler::run(sim_time duration) {
    // Time-0 delta cycle: settle DUT combinational state and commit any
    // initial NBA writes (reset etc.). No timer/monitor processing needed
    // because no timer has fired yet.
    delta_.eval();
    delta_.commit();
    delta_.end_tick();
    if (dump_fn_) dump_fn_(timer_.now());

    while (true) {
        auto nd = timer_.next_deadline();
        if (!nd) break;
        if (*nd > duration) break;

        timer_.advance_to(*nd, [&](const TimerEngine::Event& ev) {
            fire_coroutine(ev.handle, ev.wid, ev.fire_idx, ev.fire_value);
        });
        run_one_tick();
    }
}

} // namespace corosim
