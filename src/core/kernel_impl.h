#pragma once

#include "core/kernel.hpp"

namespace corosim::detail {

template <typename Fn>
inline Task make_always_edge_coro(SignalBase* sig, TriggerType edge, Fn fn) {
    while (true) {
        co_await EdgeAwaiter{sig, edge};
        fn();
    }
}

template <typename Fn>
inline Task make_always_delay_coro(sim_time interval, Fn fn) {
    fn();
    while (true) {
        co_await delay(interval);
        fn();
    }
}

} // namespace corosim::detail

namespace corosim {

// ── Kernel template members ──

template <typename TOP>
void Kernel::init(TOP* top, std::function<void(sim_time)> dump_fn) {
    dump_fn_ = std::move(dump_fn);
    top_ = top;
    sched_.set_eval_fn([top] { top->eval(); });
    sched_.set_dump_fn([this](sim_time t) {
        if (top_) set_verilator_time(t);
        if (dump_fn_) dump_fn_(t);
    });
}

template <typename Fn>
Process* Kernel::add_process(Fn&& fn) {
    return proc_mgr_.add(std::forward<Fn>(fn));
}

template <typename Fn>
void Kernel::on_pre_eval(Fn&& fn) { sched_.on_pre_eval(std::forward<Fn>(fn)); }

template <typename Fn>
void Kernel::on_post_eval(Fn&& fn) { sched_.on_post_eval(std::forward<Fn>(fn)); }

template <typename Fn>
void Kernel::on_comb(Fn&& fn) { sched_.on_comb(std::forward<Fn>(fn)); }

template <typename Trigger, typename Fn>
void Kernel::always(Trigger t, Fn fn) {
    auto info = t.trigger_info();
    if (info.type == TriggerType::DELAY) {
        add_process([interval = info.interval, fn = std::move(fn)]() -> Task {
            return detail::make_always_delay_coro(interval, std::move(fn));
        });
    } else {
        add_process([sig = info.sig, edge = info.type, fn = std::move(fn)]() -> Task {
            return detail::make_always_edge_coro(sig, edge, std::move(fn));
        });
    }
}

template <typename Fn>
void Kernel::always_comb(Fn fn) {
    sched_.on_comb(std::forward<Fn>(fn));
}

template <typename Fn, typename... Args>
void Kernel::instance(Fn&& fn, Args&&... args) {
    add_process([fn = std::forward<Fn>(fn), ...args = std::forward<Args>(args)]() -> Task {
        return fn(args...);
    });
}

template <typename Trigger, typename Fn>
void Kernel::sample(Trigger t, Fn fn) {
    auto info = t.trigger_info();
    sched_.on_pre_eval([this, sig = info.sig, edge = info.type, fn = std::move(fn)] {
        if (sched_.had_edge(sig, edge)) fn();
    });
}

template <typename Trigger, typename Fn>
void Kernel::drive(Trigger t, Fn fn) {
    auto info = t.trigger_info();
    sched_.on_post_eval([this, sig = info.sig, edge = info.type, fn = std::move(fn)] {
        if (sched_.had_edge(sig, edge)) fn();
    });
}

template <typename Trigger, typename Device>
void Kernel::device(Trigger t, Device& dev) {
    sample(t, [&dev] { dev.update_input(); });
    drive(t, [&dev] { dev.update_output(); });
}

} // namespace corosim
