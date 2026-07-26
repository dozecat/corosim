#pragma once

#include <functional>
#include "engine.hpp"

namespace corosim {

inline void always_comb(std::function<void()> fn) {
    Engine::current().on_comb(std::move(fn));
}

} // namespace corosim
