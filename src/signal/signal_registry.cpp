#include "signal/signal_registry.hpp"

#include <algorithm>

namespace corosim {

void SignalRegistry::register_signal(SignalVal* s) {
    signals_.push_back(s);
}

void SignalRegistry::unregister_signal(SignalVal* s) {
    auto it = std::find(signals_.begin(), signals_.end(), s);
    if (it != signals_.end()) signals_.erase(it);
    auto pit = std::find(pending_signals_.begin(), pending_signals_.end(), s);
    if (pit != pending_signals_.end()) pending_signals_.erase(pit);
    if (on_signal_destroyed) on_signal_destroyed(s);
}

void SignalRegistry::mark_pending(SignalVal* s) {
    pending_signals_.push_back(s);
}

} // namespace corosim
