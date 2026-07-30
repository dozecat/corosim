#include "scheduler/timer_engine.hpp"

namespace corosim {

TimerId TimerEngine::schedule(sim_time deadline, std::coroutine_handle<> h, WaitId* wid,
                              int fire_idx, int* fired) {
    TimerId tid{next_id_++};
    queue_.push({deadline, tid, h, wid, fired, fire_idx});
    return tid;
}

std::optional<sim_time> TimerEngine::next_deadline() {
    while (!queue_.empty()) {
        auto& top = queue_.top();
        if (!top.handle || top.handle.done() || !top.wid || !top.wid->valid()) {
            queue_.pop();
            continue;
        }
        return top.deadline;
    }
    return std::nullopt;
}

} // namespace corosim
