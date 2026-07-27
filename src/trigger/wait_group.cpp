#include "wait_group.hpp"
#include <algorithm>

namespace corosim {

WaitId WaitGroup::add_edge_watch(SignalBase* sig, TriggerType edge) {
    WaitId wid{++next_id_, owner_.generation, edge, sig};
    entries_.push_back({wid, true});
    return wid;
}

WaitId WaitGroup::add_delay_watch(TimerId timer) {
    WaitId wid{++next_id_, owner_.generation, TriggerType::DELAY, timer};
    entries_.push_back({wid, true});
    return wid;
}

void WaitGroup::cancel_all() {
    for (auto& e : entries_) {
        e.active = false;
        e.id.invalidate();
    }
    entries_.clear();
}

void WaitGroup::cancel_others(WaitId keep) {
    for (auto& e : entries_) {
        if (e.id.id != keep.id) {
            e.active = false;
            e.id.invalidate();
        }
    }
    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
                       [](const Entry& e) { return !e.active; }),
        entries_.end());
}

bool WaitGroup::any_active() const {
    for (auto& e : entries_) {
        if (e.active) return true;
    }
    return false;
}

} // namespace corosim
