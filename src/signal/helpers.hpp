#pragma once

#include "signal/signal.hpp"
#include "trigger/edge.hpp"

namespace corosim {

template <typename T>
EdgeAwaiter posedge(Signal<T>& sig) {
    static_assert(sizeof(T) <= 1, "posedge requires 1-byte signal type");
    return {sig.base_ptr(), TriggerInfo::POSEDGE};
}

template <typename T>
EdgeAwaiter negedge(Signal<T>& sig) {
    static_assert(sizeof(T) <= 1, "negedge requires 1-byte signal type");
    return {sig.base_ptr(), TriggerInfo::NEGEDGE};
}

template <typename T>
EdgeAwaiter change(Signal<T>& sig) {
    return {sig.base_ptr(), TriggerInfo::CHANGE};
}

} // namespace corosim
