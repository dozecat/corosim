#include "scheduler/monitor_engine.hpp"

namespace corosim {

void MonitorEngine::watch(SignalVal* sig, TriggerType type, std::coroutine_handle<> h,
                          std::shared_ptr<WaitToken> token, int fire_idx, int* fired) {
    if (!sig) return;
    FireTicket t;
    t.handle = h;
    t.token = std::move(token);
    t.fire_idx = fired;
    t.fire_value = fire_idx;
    auto& target = processing_ ? pending_ : watches_;
    target[sig].push_back({type, std::move(t)});
}

void MonitorEngine::unwatch_all(SignalVal* sig) {
    watches_.erase(sig);
    pending_.erase(sig);
}

} // namespace corosim
