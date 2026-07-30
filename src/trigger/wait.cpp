#include "trigger/wait.hpp"

namespace corosim {

WaitId* WaitGroup::add_edge_watch(SignalBase* sig, TriggerType edge) {
    cleanup_invalid();
    entries_.push_back({WaitId{++next_id_, owner_.generation, edge, sig}, true});
    return &entries_.back().id;
}

WaitId* WaitGroup::add_delay_watch(TimerId timer) {
    cleanup_invalid();
    entries_.push_back({WaitId{++next_id_, owner_.generation, TriggerType::DELAY, timer}, true});
    return &entries_.back().id;
}

void WaitGroup::cancel_all() {
    cleanup_invalid();
    for (auto& e : entries_) {
        e.id.invalidate();
    }
}

void WaitGroup::cancel_others(WaitId* keep) {
    for (auto& e : entries_) {
        if (&e.id != keep) {
            e.id.invalidate();
        }
    }
}

bool WaitGroup::has_active() const {
    for (auto& e : entries_) {
        if (e.id.valid()) return true;
    }
    return false;
}

} // namespace corosim
