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

    /**
     * @brief Fire watches for signals in @p changed that saw an edge this tick.
     *
     * Only signals that actually changed value are visited; entries are removed
     * in place (order-preserving). Firing stays inline so that any() sibling
     * cancellation (cancel_others) prevents later entries in the same vector
     * from firing in the same tick.
     */
    template <typename Fn>
    void process(const std::vector<SignalBase*>& changed, Fn&& on_fire) {
        if (changed.empty() || watches_.empty()) return;

        processing_ = true;

        for (size_t ci = 0; ci < changed.size(); ++ci) {
            SignalBase* sig = changed[ci];
            if (!sig) continue;
            auto it = watches_.find(sig);
            if (it == watches_.end()) continue;
            auto& entries = it->second;
            for (size_t i = 0; i < entries.size();) {
                auto& e = entries[i];
                if (!e.handle || e.handle.done() || !e.wid || !e.wid->valid()) {
                    entries.erase(entries.begin() + i);
                    continue;
                }
                bool triggered = false;
                switch (e.edge) {
                case TriggerInfo::POSEDGE: triggered = sig->has_posedge(); break;
                case TriggerInfo::NEGEDGE: triggered = sig->has_negedge(); break;
                case TriggerInfo::CHANGE:  triggered = sig->has_changed(); break;
                default: break;
                }
                if (!triggered) { ++i; continue; }
                on_fire(e.handle, e.wid, e.fire_idx, e.fire_value);
                // Fired entry is consumed (wid invalidated by fire_coroutine);
                // an earlier fire may also have cancelled sibling entries below,
                // so stay on the same index and let the next pass drop them.
                entries.erase(entries.begin() + i);
            }
            if (entries.empty()) watches_.erase(it);
        }

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
