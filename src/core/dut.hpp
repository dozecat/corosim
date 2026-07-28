#pragma once

#include <memory>
#include <vector>
#include "signal/signal.hpp"

namespace corosim {

template <typename TOP>
class Dut {
    TOP& top_;
    SignalRegistry& reg_;
    std::vector<std::unique_ptr<SignalBase>> owned_signals_;

public:
    Dut(TOP& top, SignalRegistry& reg) : top_(top), reg_(reg) {}

    template <typename T>
    Signal<T>& sig(T TOP::*member) {
        auto s = std::make_unique<Signal<T>>(reg_, &(top_.*member));
        auto* ptr = s.get();
        owned_signals_.push_back(std::move(s));
        return *ptr;
    }

    TOP& top() { return top_; }
};

} // namespace corosim
