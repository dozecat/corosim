#pragma once

#include <memory>
#include <functional>
#include "engine.hpp"

namespace corosim {

template <typename Trigger, typename Fn>
void always(Trigger t, Fn fn) {
    auto info = t.trigger_info();
    auto& e = Engine::current();

    if (info.type == TriggerInfo::DELAY) {
        auto interval = info.interval;
        auto next = std::make_shared<sim_time>(0);
        e.on_tick([=] {
            if (Engine::current().now() >= *next) {
                fn();
                *next = Engine::current().now() + interval;
            }
        });
    } else {
        auto sig = info.sig;
        auto edge = info.type;
        e.on_commit_eval([=] {
            if (Engine::current().had_edge(sig, edge))
                fn();
        });
    }
}

} // namespace corosim
