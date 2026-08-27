#pragma once

#include <memory>
#include <vector>

#include "signal/signal.hpp"

namespace corosim {

/**
 * @brief Binds Verilator @p TOP fields into owned Signal instances.
 * @tparam TOP Verilator-generated top module type.
 */
template <typename TOP>
class Dut {
    TOP& top_;
    SignalRegistry& signals_;
    std::vector<std::unique_ptr<SignalVal>> owned_signals_;

public:
    Dut(TOP& top, SignalRegistry& signals) : top_(top), signals_(signals) {}

    /** @brief Bind @p member of TOP as a Signal. */
    template <typename T>
    Signal<T>& sig(T TOP::*member) {
        auto s = std::make_unique<Signal<T>>(signals_, &(top_.*member));
        auto* ptr = s.get();
        owned_signals_.push_back(std::move(s));
        return *ptr;
    }

    TOP& top() { return top_; }
};

} // namespace corosim
