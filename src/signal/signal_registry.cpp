#include "signal_registry.hpp"
#include <algorithm>

namespace corosim {

void SignalRegistry::register_signal(SignalBase* s) {
    sigs_.push_back(s);
}

void SignalRegistry::unregister_signal(SignalBase* s) {
    auto it = std::find(sigs_.begin(), sigs_.end(), s);
    if (it != sigs_.end()) sigs_.erase(it);
    auto pit = std::find(pending_sigs_.begin(), pending_sigs_.end(), s);
    if (pit != pending_sigs_.end()) pending_sigs_.erase(pit);
}

void SignalRegistry::mark_pending(SignalBase* s) {
    pending_sigs_.push_back(s);
}

void SignalRegistry::commit_all() {
    for (auto* sig : pending_sigs_) {
        if (sig) sig->commit();
    }
    pending_sigs_.clear();
}

bool SignalRegistry::any_dirty() const {
    for (auto* sig : sigs_) {
        if (sig && sig->is_dirty()) return true;
    }
    return false;
}

void SignalRegistry::clear_dirty() {
    for (auto* sig : sigs_) {
        if (sig) sig->clear_dirty();
    }
}

void SignalRegistry::clear_edge_flags() {
    for (auto* sig : sigs_) {
        if (sig) sig->clear_edge_flags();
    }
}

} // namespace corosim
