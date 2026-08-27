#include "scheduler/timer_engine.hpp"

namespace corosim {

void TimerEngine::schedule(sim_time deadline, std::coroutine_handle<> h,
                           std::shared_ptr<WaitToken> token,
                           int fire_idx, int* fired) {
    FireTicket t;
    t.handle = h;
    t.token = std::move(token);
    t.fire_idx = fired;
    t.fire_value = fire_idx;
    queue_.push({deadline, next_id_++, std::move(t)});
}

std::optional<sim_time> TimerEngine::next_deadline() {
    while (!queue_.empty()) {
        auto& top = queue_.top();
        if (!top.ticket.handle || top.ticket.handle.done() ||
            !top.ticket.token || !top.ticket.token->valid()) {
            queue_.pop();
            continue;
        }
        return top.deadline;
    }
    return std::nullopt;
}

} // namespace corosim
