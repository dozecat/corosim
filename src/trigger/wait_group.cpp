#include "wait_group.hpp"

namespace corosim {

WaitId* WaitGroup::add_edge_watch(SignalBase* sig, TriggerType edge) {
    entries_.push_back({WaitId{++next_id_, owner_.generation, edge, sig}, true});
    return &entries_.back().id;
}

WaitId* WaitGroup::add_delay_watch(TimerId timer) {
    entries_.push_back({WaitId{++next_id_, owner_.generation, TriggerType::DELAY, timer}, true});
    return &entries_.back().id;
}

void WaitGroup::cancel_all() {
    for (auto& e : entries_) {
        e.id.invalidate();
    }
    // Never erase: scheduler holds WaitId* pointers into this list.
    // Entries are destroyed with the WaitGroup when Process is destroyed.
}

void WaitGroup::cancel_others(WaitId* keep) {
    for (auto& e : entries_) {
        if (&e.id != keep) {
            e.id.invalidate();
        }
    }
    // Never erase: scheduler holds WaitId* pointers into this list.
}

bool WaitGroup::any_active() const {
    for (auto& e : entries_) {
        if (e.id.valid()) return true;
    }
    return false;
}

} // namespace corosim
