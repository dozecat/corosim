#pragma once

#include "signal/signal.hpp"
#include "trigger/trigger_awaiter.hpp"

namespace corosim {

/** @brief Rising-edge awaiter; @p T must be a 1-byte signal type. */
template <typename T>
TriggerAwaiter posedge(Signal<T>& sig) {
    static_assert(sizeof(T) <= 1, "posedge requires 1-byte signal type");
    return {sig.as_val(), TriggerType::POSEDGE};
}

/** @brief Falling-edge awaiter; @p T must be a 1-byte signal type. */
template <typename T>
TriggerAwaiter negedge(Signal<T>& sig) {
    static_assert(sizeof(T) <= 1, "negedge requires 1-byte signal type");
    return {sig.as_val(), TriggerType::NEGEDGE};
}

/** @brief Value-change awaiter for any Signal<T>. */
template <typename T>
TriggerAwaiter change(Signal<T>& sig) {
    return {sig.as_val(), TriggerType::CHANGE};
}

} // namespace corosim
