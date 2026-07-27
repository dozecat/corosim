#pragma once

#include <functional>
#include "../core/kernel.hpp"

namespace corosim {

inline void always_comb(std::function<void()> fn) {
    auto* k = detail::current_kernel();
    if (k) k->on_comb(std::move(fn));
}

} // namespace corosim
