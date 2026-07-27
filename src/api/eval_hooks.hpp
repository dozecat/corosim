#pragma once

#include <functional>
#include <memory>
#include "../core/types.hpp"
#include "../core/kernel.hpp"

namespace corosim {

inline void pre_eval(std::function<void()> fn) {
    auto* k = detail::current_kernel();
    if (k) k->on_pre_eval(std::move(fn));
}

inline void post_eval(std::function<void()> fn) {
    auto* k = detail::current_kernel();
    if (k) k->on_post_eval(std::move(fn));
}

} // namespace corosim
