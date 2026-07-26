#pragma once

#include <cstdint>

namespace corosim {

using sim_time = uint64_t;

struct TriggerInfo {
    enum Type : int { DELAY = 0, POSEDGE, NEGEDGE, CHANGE };
    Type type;
    void* sig;
    sim_time interval;
};

} // namespace corosim
