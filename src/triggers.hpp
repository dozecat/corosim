#pragma once

#include <array>
#include <utility>

#include "types.hpp"
#include "engine.hpp"

namespace corosim {

template <typename... Triggers>
auto any(Triggers&&... triggers) {
    std::array<TriggerInfo, sizeof...(Triggers)> infos = {{triggers.trigger_info()...}};

    struct Awaiter {
        std::array<TriggerInfo, sizeof...(Triggers)> infos;

        bool await_ready() noexcept { return false; }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            auto& e = Engine::current();
            for (auto& info : infos) {
                switch (info.type) {
                case TriggerInfo::POSEDGE:
                case TriggerInfo::NEGEDGE:
                case TriggerInfo::CHANGE:
                    e.watch_edge(info.sig, info.type, h);
                    break;
                case TriggerInfo::DELAY:
                    e.watch_delay(h, e.now() + info.interval);
                    break;
                }
            }
        }

        void await_resume() noexcept {}
    };

    return Awaiter{infos};
}

} // namespace corosim
