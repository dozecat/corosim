#pragma once

#include <cstdint>
#include <variant>

namespace corosim {

using sim_time = uint64_t;

class SignalBase;

struct TriggerInfo {
    enum Type : int { DELAY = 0, POSEDGE, NEGEDGE, CHANGE };
    Type type;
    SignalBase* sig;      // was void* — now type-safe
    sim_time interval;
};

// Alias for new-style code (enum class forward compat for P2)
// For Phase 1, TriggerInfo::Type serves both old and new code.
using TriggerType = TriggerInfo::Type;

struct TimerId { uint64_t id; };

using WaitTarget = std::variant<SignalBase*, TimerId>;

struct ProcessId {
    uint64_t id;
    uint32_t generation;

    bool operator==(const ProcessId& o) const = default;
    bool operator!=(const ProcessId& o) const = default;
};

struct WaitId {
    uint64_t id;
    uint32_t process_generation;
    TriggerType type;
    WaitTarget target;

    bool valid() const { return id != 0; }
    void invalidate() { id = 0; }
};

} // namespace corosim
