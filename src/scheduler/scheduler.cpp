#include "scheduler.hpp"

#include <stdexcept>
#include <string>

#include <verilated.h>

namespace corosim {

void Scheduler::fire_coroutine(const FireTicket& t) {
    if (t.fire_idx) {
        *t.fire_idx = t.fire_value;
    }
    if (cancel_others_fn_) {
        cancel_others_fn_(t.handle, t.token);
    }
    if (t.token) {
        t.token->invalidate();
    }
    t.handle.resume();
}

void Scheduler::settle() {
    constexpr size_t kMaxDeltaCycles = 10000;

    for (size_t cycle = 0; cycle < kMaxDeltaCycles; ++cycle) {
        delta_.eval();
        delta_.observe();
        monitor_.process(delta_.changed(), [this](const FireTicket& t) { fire_coroutine(t); });

        const bool has_followup = delta_.has_pending();
        delta_.apply_pending();
        if (!has_followup) {
            delta_.end_tick();
            return;
        }
    }

    delta_.end_tick();
    throw std::runtime_error("corosim: delta cycle limit exceeded at time " +
                             std::to_string(timer_.now()));
}

void Scheduler::run_one_tick() {
    if (time_hook_) {
        time_hook_(timer_.now());
    }

    settle();

    if (dump_fn_) {
        dump_fn_(timer_.now());
    }
}

void Scheduler::run(sim_time duration) {
    // t=0: settle initial combinational state and establish prev_val baselines.
    if (time_hook_) {
        time_hook_(timer_.now());
    }
    settle();
    if (dump_fn_) {
        dump_fn_(timer_.now());
    }

    while (true) {
        if (Verilated::gotFinish()) {
            break;
        }

        auto nd = timer_.next_deadline();
        if (!nd) {
            break;
        }
        if (*nd > duration) {
            break;
        }

        timer_.advance_to(*nd, [this](const FireTicket& t) { fire_coroutine(t); });
        run_one_tick();

        if (Verilated::gotFinish()) {
            break;
        }
    }
}

} // namespace corosim
