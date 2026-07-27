#pragma once

#include <functional>
#include "../process/proc.hpp"
#include "../core/kernel.hpp"

namespace corosim {

template <typename Fn>
void proc(Fn&& fn) {
    auto* k = detail::current_kernel();
    if (k) k->add_process(std::forward<Fn>(fn));
}

} // namespace corosim
