#pragma once

#include <coroutine>
#include <type_traits>
#include "../core/types.hpp"
#include "../signal/signal_base.hpp"
#include "internal.hpp"

namespace corosim {

template <typename T> class Signal;

template <typename T>
class Posedge {
    static_assert(sizeof(T) <= 1, "posedge/negedge requires 1-byte signal type");

public:
    explicit Posedge(const Signal<T>& sig) : sig_base_(static_cast<SignalBase*>(const_cast<Signal<T>*>(&sig))) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_wait(sig_base_, TriggerType::POSEDGE, h);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerType::POSEDGE, sig_base_, 0};
    }

private:
    SignalBase* sig_base_;
};

template <typename T>
class Negedge {
    static_assert(sizeof(T) <= 1, "posedge/negedge requires 1-byte signal type");

public:
    explicit Negedge(const Signal<T>& sig) : sig_base_(static_cast<SignalBase*>(const_cast<Signal<T>*>(&sig))) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_wait(sig_base_, TriggerType::NEGEDGE, h);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerType::NEGEDGE, sig_base_, 0};
    }

private:
    SignalBase* sig_base_;
};

template <typename T>
class Change {
public:
    explicit Change(const Signal<T>& sig) : sig_base_(static_cast<SignalBase*>(const_cast<Signal<T>*>(&sig))) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_wait(sig_base_, TriggerType::CHANGE, h);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerType::CHANGE, sig_base_, 0};
    }

private:
    SignalBase* sig_base_;
};

} // namespace corosim
