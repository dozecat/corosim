#include "signal_registry.hpp"

namespace corosim {

void SignalRegistry::register_signal(SignalBase* s) {
    sigs_.push_back(s);
}

void SignalRegistry::commit_all() {
    for (auto* sig : sigs_) {
        if (sig) sig->commit();
    }
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
