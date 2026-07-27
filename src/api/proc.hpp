#pragma once

#include "../core/kernel.hpp"
#include "initial.hpp"

namespace corosim {

// Legacy: factory lambda style — proc([&]() -> Proc { return my_coro(args...); })
template <typename Fn>
void proc(Fn&& fn) {
    auto* k = detail::current_kernel();
    if (k) k->add_process(std::forward<Fn>(fn));
}

// Convenience: register a coroutine function with arguments directly
//   proc(reset_func, &rst, &wr_clk);
template <typename Fn, typename... Args>
void proc(Fn&& fn, Args&&... args) {
    initial(std::forward<Fn>(fn), std::forward<Args>(args)...);
}

} // namespace corosim
