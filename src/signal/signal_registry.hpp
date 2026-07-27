#pragma once

#include <vector>
#include "signal_base.hpp"

namespace corosim {

class SignalRegistry {
public:
    void register_signal(SignalBase* s);

    void commit_all();
    bool any_dirty() const;
    void clear_dirty();
    void clear_edge_flags();

    // Return all known signals (for iteration in delta loop)
    std::vector<SignalBase*>& dirty_signals() { return sigs_; }
    const std::vector<SignalBase*>& dirty_signals() const { return sigs_; }

private:
    std::vector<SignalBase*> sigs_;
};

} // namespace corosim
