#include "scheduler/monitor_engine.hpp"

namespace corosim {

void MonitorEngine::watch(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h,
                           WaitId* wid, int fire_idx, int* fired) {
    if (!sig) return;
    auto& target = processing_ ? pending_ : watches_;
    target[sig].push_back({edge, h, wid, fired, fire_idx});
}

void MonitorEngine::unwatch_all(SignalBase* sig) {
    watches_.erase(sig);
    pending_.erase(sig);
}

} // namespace corosim
