#pragma once

#include <coroutine>
#include <functional>
#include <unordered_map>
#include <vector>
#include "core/types.hpp"
#include "signal/signal_base.hpp"

namespace corosim {

class MonitorEngine {
public:
    void watch(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h,
               WaitId* wid, int fire_idx = -1, int* fired = nullptr);

    void unwatch_all(SignalBase* sig);

    template <typename Fn>
    void process(Fn&& on_fire) {
        if (watches_.empty()) return;

        processing_ = true;
        decltype(watches_) kept;

        for (auto& [sig, entries] : watches_) {
            bool has_pos = sig->has_posedge();
            bool has_neg = sig->has_negedge();
            bool has_chg = sig->has_changed();

            if (!has_pos && !has_neg && !has_chg) {
                kept[sig] = std::move(entries);
                continue;
            }

            std::vector<MonitorEntry> live;
            for (auto& e : entries) {
                if (!e.handle || e.handle.done() || !e.wid || !e.wid->valid())
                    continue;
                bool triggered = false;
                switch (e.edge) {
                case TriggerInfo::POSEDGE: triggered = has_pos; break;
                case TriggerInfo::NEGEDGE: triggered = has_neg; break;
                case TriggerInfo::CHANGE:  triggered = has_chg; break;
                default: break;
                }
                if (triggered) {
                    on_fire(e.handle, e.wid, e.fire_idx, e.fire_value);
                } else {
                    live.push_back(std::move(e));
                }
            }
            if (!live.empty())
                kept[sig] = std::move(live);
        }

        watches_ = std::move(kept);
        processing_ = false;

        for (auto& [sig, vec] : pending_)
            for (auto& e : vec)
                watches_[sig].push_back(std::move(e));
        pending_.clear();
    }

private:
    struct MonitorEntry {
        TriggerType edge;
        std::coroutine_handle<> handle;
        WaitId* wid = nullptr;
        int* fire_idx = nullptr;
        int fire_value = -1;
    };

    bool processing_ = false;
    std::unordered_map<SignalBase*, std::vector<MonitorEntry>> watches_;
    std::unordered_map<SignalBase*, std::vector<MonitorEntry>> pending_;
};

} // namespace corosim
