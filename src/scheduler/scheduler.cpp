#include "scheduler.hpp"

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

void Scheduler::run_one_tick() {
    if (time_hook_) {
        time_hook_(timer_.now());
    }

    timer_.advance_to(timer_.now(), [this](const FireTicket& t) { fire_coroutine(t); });

    delta_.eval();          // apply_pending → sample → top.eval() → drive → apply_pending
    delta_.observe();       // full scan: changed_ + update_prev (DUT changes here)
    monitor_.process(delta_.changed(), [this](const FireTicket& t) { fire_coroutine(t); });

    delta_.apply_pending(); // post-monitor commit: same-tick into VCD; no observe
    delta_.end_tick();

    if (dump_fn_) {
        dump_fn_(timer_.now());
    }
}

void Scheduler::run(sim_time duration) {
    // t=0: settle initial combinational state and establish prev_val baselines.
    if (time_hook_) {
        time_hook_(timer_.now());
    }
    delta_.eval();
    delta_.observe();
    delta_.end_tick();
    if (dump_fn_) {
        dump_fn_(timer_.now());
    }

    while (true) {
        auto nd = timer_.next_deadline();
        if (!nd) {
            break;
        }
        if (*nd > duration) {
            break;
        }

        timer_.advance_to(*nd, [this](const FireTicket& t) { fire_coroutine(t); });
        run_one_tick();
    }
}

} // namespace corosim
