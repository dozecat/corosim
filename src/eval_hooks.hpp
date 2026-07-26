#pragma once

#include <memory>
#include <functional>
#include "engine.hpp"

namespace corosim {

inline void pre_eval(std::function<void()> fn) {
    Engine::current().on_pre_eval(std::move(fn));
}

inline void post_eval(std::function<void()> fn) {
    Engine::current().on_post_eval(std::move(fn));
}

template <typename Trigger, typename Fn>
void sample(Trigger t, Fn fn) {
    auto info = t.trigger_info();
    auto ready = std::make_shared<bool>(false);
    auto& e = Engine::current();

    e.on_commit([=] {
        if (!*ready && e.had_edge(info.sig, info.type))
            *ready = true;
    });
    e.on_pre_eval([=] {
        if (*ready) { fn(); *ready = false; }
    });
    e.on_tick_end([=] { *ready = false; });
}

template <typename Trigger, typename Fn>
void drive(Trigger t, Fn fn) {
    auto info = t.trigger_info();
    auto ready = std::make_shared<bool>(false);
    auto& e = Engine::current();

    e.on_commit_eval([=] {
        if (!*ready && e.had_edge(info.sig, info.type))
            *ready = true;
    });
    e.on_post_eval([=] {
        if (*ready) { fn(); *ready = false; }
    });
    e.on_tick_end([=] { *ready = false; });
}

} // namespace corosim
