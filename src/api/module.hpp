#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#include "signal/signal.hpp"
#include "core/kernel.hpp"
#include "trigger/delay.hpp"
#include "trigger/helpers.hpp"
#include "coroutine/coroutine.hpp"

namespace corosim {

/**
 * @brief Named hierarchical TB component.
 *
 * Call attach() to bind a Kernel, then override build() to register signals
 * and coroutines.
 */
class Module {
public:
    explicit Module(std::string_view name) : name_(name) {}
    virtual ~Module() = default;

    /** @brief Called after attach(); override to build hierarchy. */
    virtual void build() {}

    const std::string& name() const { return name_; }
    Kernel& kernel() { return *kernel_; }

    /** @brief Bind @p kernel, recurse to children, then call build(). */
    void attach(Kernel& kernel) {
        kernel_ = &kernel;
        for (auto* child : children_) child->attach(kernel);
        build();
    }

    /** @brief Create an owned Signal, optionally bound to @p hdl_ptr. */
    template <typename T>
    Signal<T>& sig(T* hdl_ptr = nullptr) {
        auto signal = std::make_unique<Signal<T>>(kernel_->signals(), hdl_ptr);
        auto* ptr = signal.get();
        owned_signals_.push_back(std::move(signal));
        return *ptr;
    }

    /** @brief Construct and own a child module of type @p M. */
    template <typename M, typename... Args>
    M& create_module(std::string_view name, Args&&... args) {
        auto mod = std::make_unique<M>(name, std::forward<Args>(args)...);
        auto* ptr = mod.get();
        children_.push_back(ptr);
        owned_modules_.push_back(std::move(mod));
        return *ptr;
    }

    template <typename TriggerT, typename Fn>
    void always(TriggerT t, Fn fn) { kernel_->always(t, std::move(fn)); }

    template <typename Fn, typename... Args>
    Coroutine* instance(Fn&& fn, Args&&... args) {
        return kernel_->instance(std::forward<Fn>(fn), std::forward<Args>(args)...);
    }

    template <typename TriggerT, typename Fn>
    Coroutine* check(TriggerT t, Fn fn) {
        return kernel_->check(t, std::move(fn));
    }

    template <typename TriggerT, typename Fn>
    void sample(TriggerT t, Fn fn) { kernel_->sample(t, std::move(fn)); }

    template <typename TriggerT, typename Fn>
    void drive(TriggerT t, Fn fn) { kernel_->drive(t, std::move(fn)); }

private:
    std::string name_;
    Kernel* kernel_ = nullptr;
    std::vector<std::unique_ptr<SignalVal>> owned_signals_;
    std::vector<Module*> children_;
    std::vector<std::unique_ptr<Module>> owned_modules_;
};

} // namespace corosim
