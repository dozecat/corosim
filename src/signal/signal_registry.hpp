#pragma once

#include <vector>
#include "signal_base.hpp"

namespace corosim {

class SignalRegistry {
public:
    void register_signal(SignalBase* s);
    void unregister_signal(SignalBase* s);
    void mark_pending(SignalBase* s);

    void commit_all();
    bool any_dirty() const;
    void clear_dirty();
    void clear_edge_flags();

    std::vector<SignalBase*>& dirty_signals() { return sigs_; }
    const std::vector<SignalBase*>& dirty_signals() const { return sigs_; }

private:
    std::vector<SignalBase*> sigs_;
    std::vector<SignalBase*> pending_sigs_;
};

} // namespace corosim
