#pragma once

#include <memory>
#include <string>
#include <vector>
#include "signal/signal.hpp"

namespace corosim {

class Kernel;

class Module {
public:
    explicit Module(std::string_view name) : name_(name) {}
    virtual ~Module() = default;

    virtual void build() {}

    const std::string& name() const { return name_; }
    Kernel& sim() { return *sim_; }

    void attach(Kernel& sim) {
        sim_ = &sim;
        for (auto* child : children_) child->attach(sim);
    }

    template <typename T>
    Signal<T>& reg_sig(T* hdl_ptr = nullptr) {
        auto sig = std::make_unique<Signal<T>>(sim_->signals(), hdl_ptr);
        auto* ptr = sig.get();
        owned_signals_.push_back(std::move(sig));
        return *ptr;
    }

    template <typename M, typename... Args>
    M& create_module(std::string_view name, Args&&... args) {
        auto mod = std::make_unique<M>(name, std::forward<Args>(args)...);
        auto* ptr = mod.get();
        children_.push_back(ptr);
        owned_modules_.push_back(std::move(mod));
        return *ptr;
    }

private:
    std::string name_;
    Kernel* sim_ = nullptr;
    std::vector<std::unique_ptr<SignalBase>> owned_signals_;
    std::vector<Module*> children_;
    std::vector<std::unique_ptr<Module>> owned_modules_;
};

} // namespace corosim
