#pragma once

#include <list>
#include "core/types.hpp"

namespace corosim {

class WaitGroup {
public:
    explicit WaitGroup(ProcessId owner) : owner_(owner) {}

    WaitId* add_edge_watch(SignalBase* sig, TriggerType edge);
    WaitId* add_delay_watch(TimerId timer);

    void cancel_all();
    void cancel_others(WaitId* keep);

    bool has_active() const;

private:
    struct Entry {
        WaitId id;
        bool active = true;
    };

    void cleanup_invalid() {
        entries_.remove_if([](const Entry& e) { return !e.id.valid(); });
    }

    ProcessId owner_;
    std::list<Entry> entries_;
    uint64_t next_id_ = 0;
};

} // namespace corosim
