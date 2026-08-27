#pragma once

#include <coroutine>
#include <memory>
#include <unordered_map>
#include <vector>

#include "trigger/spec.hpp"
#include "signal/signal_val.hpp"
#include "coroutine/wait.hpp"
#include "scheduler/fire_ticket.hpp"

namespace corosim {

/** @brief One observed change: a signal plus its trigger info for the tick. */
struct ChangeRecord {
    SignalVal* sig;
    TrigInfo trig;
};

class MonitorEngine {
public:
    void watch(SignalVal* sig, TriggerType type, std::coroutine_handle<> h,
               std::shared_ptr<WaitToken> token, int fire_idx = -1, int* fired = nullptr);

    void unwatch_all(SignalVal* sig);

    /**
     * @brief Fire watches for signals in @p changed that saw a trigger this tick.
     *
     * Entries are removed in place (order-preserving). Firing stays inline so
     * that any() sibling cancellation (cancel_others) prevents later entries in
     * the same vector from firing in the same tick.
     */
    template <typename Fn>
    void process(const std::vector<ChangeRecord>& changed, Fn&& on_fire) {
        if (changed.empty() || watches_.empty()) return;

        processing_ = true;

        for (size_t ci = 0; ci < changed.size(); ++ci) {
            SignalVal* sig = changed[ci].sig;
            if (!sig) continue;
            auto it = watches_.find(sig);
            if (it == watches_.end()) continue;
            auto& entries = it->second;
            const auto& trig = changed[ci].trig;
            for (size_t i = 0; i < entries.size();) {
                auto& e = entries[i];
                if (!e.ticket.handle || e.ticket.handle.done() ||
                    !e.ticket.token || !e.ticket.token->valid()) {
                    entries.erase(entries.begin() + i);
                    continue;
                }
                bool triggered = false;
                switch (e.type) {
                case TriggerType::POSEDGE: triggered = trig.posedge; break;
                case TriggerType::NEGEDGE: triggered = trig.negedge; break;
                case TriggerType::CHANGE:  triggered = trig.changed; break;
                default: break;
                }
                if (!triggered) { ++i; continue; }
                on_fire(e.ticket);
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
        TriggerType type;
        FireTicket ticket;
    };

    bool processing_ = false;
    std::unordered_map<SignalVal*, std::vector<MonitorEntry>> watches_, pending_;
};

} // namespace corosim
