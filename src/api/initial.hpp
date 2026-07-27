#pragma once

#include "../core/kernel.hpp"

namespace corosim {

template <typename Fn, typename... Args>
void initial(Fn&& fn, Args&&... args) {
    auto* k = detail::current_kernel();
    if (!k) return;
    k->add_process([fn = std::forward<Fn>(fn), ...args = std::forward<Args>(args)]() -> Proc {
        return fn(args...);
    });
}

} // namespace corosim
