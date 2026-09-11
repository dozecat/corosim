#pragma once

#include <functional>
#include <vector>

#include "signal/signal_val.hpp"

namespace corosim {

/**
 * @brief Signal membership manager with two tables: all + pending.
 *
 *   signals()   -- all registered signals (observe scans this, O(N))
 *   pending()   -- signals with a pending NBA (apply scans only this, O(P))
 */
class SignalRegistry {
public:
    void add(SignalVal* s);
    void remove(SignalVal* s);                     // also removes from pending_
    void mark_pending(SignalVal* s);               // called by Signal::next()

    std::vector<SignalVal*>& signals() { return signals_; }
    const std::vector<SignalVal*>& signals() const { return signals_; }
    std::vector<SignalVal*>& pending() { return pending_; }
    const std::vector<SignalVal*>& pending() const { return pending_; }

    std::function<void(SignalVal*)> on_destroyed;  // → Scheduler::on_signal_destroy

private:
    std::vector<SignalVal*> signals_;
    std::vector<SignalVal*> pending_;
};

} // namespace corosim
