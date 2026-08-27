#pragma once

#include <functional>
#include <vector>

#include "trigger/spec.hpp"
#include "signal/signal_val.hpp"
#include "signal/signal_registry.hpp"
#include "scheduler/monitor_engine.hpp"

namespace corosim {

/**
 * @brief Three-phase eval orchestration + NBA commit + change/trigger observation.
 *
 *   eval():      apply_pending → pre hooks(sample) → eval_fn → post hooks(drive) → apply_pending
 *   observe():   full scan of signals(): trig_info() → changed_ + update_prev()
 *                (captures DUT-driven changes here)
 *   end_tick():  changed_.clear()
 */
class DeltaEngine {
public:
    explicit DeltaEngine(SignalRegistry& signals) : signals_(signals) {}

    void set_eval_fn(std::function<void()> fn) { eval_fn_ = std::move(fn); }

    template <typename Fn>
    void on_pre_eval(Fn&& fn) { pre_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }
    template <typename Fn>
    void on_post_eval(Fn&& fn) { post_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }

    /** @brief Commit pending NBAs; scans only Registry::pending(), O(P). Does not observe. */
    void apply_pending() {
        auto& pend = signals_.pending();
        for (auto* s : pend) if (s) s->apply_next();
        pend.clear();
    }

    void eval() {
        apply_pending();
        for (auto& h : pre_eval_hooks_) if (h) h();
        if (eval_fn_) eval_fn_();
        for (auto& h : post_eval_hooks_) if (h) h();
        apply_pending();
    }

    /** @brief Full scan: build changed_ records and advance prev_val. */
    void observe() {
        changed_.clear();
        for (auto* s : signals_.signals()) {
            if (!s) continue;
            auto trig = s->trig_info();
            if (trig.changed) changed_.push_back({s, trig});
            s->update_prev();
        }
    }

    void end_tick() { changed_.clear(); }

    /** @brief Query whether @p sig currently satisfies trigger @p t (pure). */
    bool triggered(SignalVal* sig, TriggerType t) const {
        if (!sig) return false;
        auto trig = sig->trig_info();
        switch (t) {
        case TriggerType::POSEDGE: return trig.posedge;
        case TriggerType::NEGEDGE: return trig.negedge;
        case TriggerType::CHANGE:  return trig.changed;
        }
        return false;
    }

    const std::vector<ChangeRecord>& changed() const { return changed_; }

private:
    SignalRegistry& signals_;
    std::function<void()> eval_fn_;
    std::vector<std::function<void()>> pre_eval_hooks_, post_eval_hooks_;
    std::vector<ChangeRecord> changed_;
};

} // namespace corosim
