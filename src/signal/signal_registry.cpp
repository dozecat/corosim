#include "signal/signal_registry.hpp"

#include <algorithm>

namespace corosim {

void SignalRegistry::add(SignalVal* s) {
    signals_.push_back(s);
}

void SignalRegistry::remove(SignalVal* s) {
    auto it = std::find(signals_.begin(), signals_.end(), s);
    if (it != signals_.end()) {
        signals_.erase(it);
    }
    auto pit = std::find(pending_.begin(), pending_.end(), s);
    if (pit != pending_.end()) {
        pending_.erase(pit);
    }
    if (on_destroyed) {
        on_destroyed(s);
    }
}

void SignalRegistry::mark_pending(SignalVal* s) {
    pending_.push_back(s);
}

} // namespace corosim
