/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        signal.hpp
 * @brief       Signal wrappers and edge/change trigger types
 * @see         https://github.com/dozecat/corosim
 *
 * @details     SignalBase provides the engine interface for NBA commit and
 *              edge detection. Signal<T> wraps a Verilator signal pointer
 *              with non-blocking assignment (next/read). Free functions
 *              (posedge/negedge/change) create trigger/awaiter objects.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/25  Initial release
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <cstring>
#include <coroutine>
#include <vector>
#include <verilated.h>

namespace corosim {

class Engine;

namespace detail {
void register_edge_watcher(void* sig_base, int edge, std::coroutine_handle<> h);
}

enum EdgeType : int { POSEDGE = 0, NEGEDGE = 1, CHANGE = 2 };

class SignalBase {
public:
    virtual ~SignalBase() = default;
    virtual void commit() = 0;
    virtual bool is_dirty() const = 0;
    virtual void clear_dirty() = 0;
    virtual bool had_posedge() const = 0;
    virtual bool had_negedge() const = 0;
    virtual void clear_edge_flags() = 0;
};

class SignalRegistry {
public:
    static std::vector<SignalBase*>& all() {
        static std::vector<SignalBase*> reg;
        return reg;
    }
};

template <typename T>
class Signal : public SignalBase {
public:
    explicit Signal(T* ptr) : ptr(ptr), prev_val(*ptr) {
        SignalRegistry::all().push_back(this);
    }

    T read() const { return *ptr; }
    operator T() const { return read(); }

    void next(T val) {
        next_val = val;
        pending = true;
    }

    void commit() override {
        if (pending) {
            prev_val = *ptr;
            *ptr = next_val;
            if (!prev_val && *ptr) posedge_flag = true;
            if (prev_val && !*ptr) negedge_flag = true;
            dirty = (prev_val != *ptr);
            pending = false;
        }
    }

    bool is_dirty() const override { return dirty; }
    void clear_dirty() override { dirty = false; }
    bool had_posedge() const override { return posedge_flag; }
    bool had_negedge() const override { return negedge_flag; }

    void clear_edge_flags() override {
        posedge_flag = false;
        negedge_flag = false;
    }

private:
    T*   ptr;
    T    prev_val;
    T    next_val;
    bool pending = false;
    bool dirty  = false;
    bool posedge_flag = false;
    bool negedge_flag = false;
};

// ---- Signal<VlWide<N>> specialization (65+ bit signals) ----
template <int N>
class Signal<VlWide<N>> : public SignalBase {
public:
    explicit Signal(VlWide<N>* ptr) : ptr(ptr) {
        std::memcpy(prev_val.m_storage, ptr->m_storage, sizeof(uint32_t) * N);
        SignalRegistry::all().push_back(this);
    }

    const uint32_t* read() const { return ptr->m_storage; }

    void next(const VlWide<N>& val) {
        pending = true;
        next_val = val;
    }

    void commit() override {
        if (pending) {
            std::memcpy(prev_val.m_storage, ptr->m_storage, sizeof(uint32_t) * N);
            *ptr = next_val;
            dirty = std::memcmp(prev_val.m_storage, ptr->m_storage, sizeof(uint32_t) * N) != 0;
            pending = false;
        }
    }

    bool is_dirty() const override { return dirty; }
    void clear_dirty() override { dirty = false; }
    bool had_posedge() const override { return false; }
    bool had_negedge() const override { return false; }
    void clear_edge_flags() override {}

private:
    VlWide<N>*  ptr;
    VlWide<N>   prev_val;
    VlWide<N>   next_val;
    bool pending = false;
    bool dirty  = false;
};

// ---- Signal<bool> specialization: accepts uint8_t* (Verilator CData) ----
template <>
class Signal<bool> : public SignalBase {
public:
    explicit Signal(uint8_t* ptr) : ptr(ptr), prev_val(*ptr) {
        SignalRegistry::all().push_back(this);
    }

    bool read() const { return *ptr != 0; }
    operator bool() const { return read(); }

    void next(bool val) {
        next_val = val ? 1 : 0;
        pending = true;
    }

    void commit() override {
        if (pending) {
            prev_val = *ptr;
            *ptr = next_val;
            if (!prev_val && *ptr) posedge_flag = true;
            if (prev_val && !*ptr) negedge_flag = true;
            dirty = (prev_val != *ptr);
            pending = false;
        }
    }

    bool is_dirty() const override { return dirty; }
    void clear_dirty() override { dirty = false; }
    bool had_posedge() const override { return posedge_flag; }
    bool had_negedge() const override { return negedge_flag; }

    void clear_edge_flags() override {
        posedge_flag = false;
        negedge_flag = false;
    }

private:
    uint8_t* ptr;
    uint8_t  prev_val;
    uint8_t  next_val;
    bool pending = false;
    bool dirty  = false;
    bool posedge_flag = false;
    bool negedge_flag = false;
};

template <typename T>
class Posedge {
    static_assert(sizeof(T) <= 1, "posedge/negedge supports 1-byte signals only");
public:
    explicit Posedge(Signal<T>& sig) : sig_(&sig) {}

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_watcher(sig_, POSEDGE, h);
    }
    void await_resume() noexcept {}

    Signal<T>& signal() const { return *sig_; }

private:
    Signal<T>* sig_;
};

template <typename T>
class Negedge {
    static_assert(sizeof(T) <= 1, "posedge/negedge supports 1-byte signals only");
public:
    explicit Negedge(Signal<T>& sig) : sig_(&sig) {}

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_watcher(sig_, NEGEDGE, h);
    }
    void await_resume() noexcept {}

    Signal<T>& signal() const { return *sig_; }

private:
    Signal<T>* sig_;
};

template <typename T>
class Change {
public:
    explicit Change(Signal<T>& sig) : sig_(&sig) {}

    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> h) {
        detail::register_edge_watcher(sig_, CHANGE, h);
    }
    void await_resume() noexcept {}

    Signal<T>& signal() const { return *sig_; }

private:
    Signal<T>* sig_;
};

template <typename T>
Posedge<T> posedge(Signal<T>& sig) { return Posedge<T>(sig); }

template <typename T>
Negedge<T> negedge(Signal<T>& sig) { return Negedge<T>(sig); }

template <typename T>
Change<T> change(Signal<T>& sig) { return Change<T>(sig); }

} // namespace corosim
