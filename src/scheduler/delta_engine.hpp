#pragma once

#include <functional>
#include <vector>
#include "core/types.hpp"
#include "signal/signal_base.hpp"
#include "signal/signal_registry.hpp"

namespace corosim {

class DeltaEngine {
public:
    explicit DeltaEngine(SignalRegistry& signals) : signals_(signals) {}

    void set_eval_fn(std::function<void()> fn) { eval_fn_ = std::move(fn); }

    template <typename Fn>
    void on_pre_eval(Fn&& fn) { pre_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }
    template <typename Fn>
    void on_post_eval(Fn&& fn) { post_eval_hooks_.emplace_back(std::forward<Fn>(fn)); }

    bool had_edge(SignalBase* sig, TriggerType edge) const {
        switch (edge) {
        case TriggerInfo::POSEDGE: return sig->has_posedge();
        case TriggerInfo::NEGEDGE: return sig->has_negedge();
        case TriggerInfo::CHANGE:  return sig->has_changed();
        default: return false;
        }
    }

    void eval() {
        signals_.commit_all();
        for (auto& h : pre_eval_hooks_) if (h) h();
        if (eval_fn_) eval_fn_();
        for (auto& h : post_eval_hooks_) if (h) h();
        signals_.commit_all();
    }

    void commit() {
        signals_.commit_all();
    }

    void end_tick() {
        signals_.clear_edge_flags();
    }

private:
    SignalRegistry& signals_;
    std::function<void()> eval_fn_;
    std::vector<std::function<void()>> pre_eval_hooks_;
    std::vector<std::function<void()>> post_eval_hooks_;
};

} // namespace corosim
