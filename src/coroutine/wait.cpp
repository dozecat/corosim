#include "coroutine/wait.hpp"

#include <algorithm>

namespace corosim {

std::shared_ptr<WaitToken> WaitGroup::add_trigger_watch(SignalVal* sig, TriggerType type) {
    (void)sig;
    (void)type;
    cleanup_invalid();
    auto token = std::make_shared<WaitToken>();
    entries_.push_back(token);
    return token;
}

std::shared_ptr<WaitToken> WaitGroup::add_delay_watch() {
    cleanup_invalid();
    auto token = std::make_shared<WaitToken>();
    entries_.push_back(token);
    return token;
}

void WaitGroup::cancel_all() {
    for (auto& e : entries_) {
        if (e) {
            e->invalidate();
        }
    }
}

void WaitGroup::cancel_others(const std::shared_ptr<WaitToken>& keep) {
    for (auto& e : entries_) {
        if (e && e != keep) {
            e->invalidate();
        }
    }
}

void WaitGroup::cleanup_invalid() {
    entries_.erase(
        std::remove_if(entries_.begin(), entries_.end(),
                       [](const std::shared_ptr<WaitToken>& e) { return !e || !e->valid(); }),
        entries_.end());
}

} // namespace corosim
