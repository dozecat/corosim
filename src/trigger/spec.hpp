#pragma once

#include <cstdint>
#include <variant>

#include "core/types.hpp"

namespace corosim {

class SignalVal;

/** @brief Signal trigger kind: rising / falling / any change. */
enum class TriggerType : uint8_t { POSEDGE, NEGEDGE, CHANGE };

/** @brief Wait spec for a delay of @p interval time units. */
struct DelaySpec { sim_time interval; };

/** @brief Wait spec for a signal trigger of a given kind. */
struct TriggerSpec { SignalVal* sig; TriggerType type; };

/** @brief Unified wait trigger: a delay or a signal trigger. */
using Trigger = std::variant<DelaySpec, TriggerSpec>;

} // namespace corosim
