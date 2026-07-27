#pragma once

#include <cstdint>
#include <cstring>
#include <verilated.h>

#include "signal/signal_base.hpp"
#include "signal/signal_registry.hpp"
#include "core/types.hpp"
#include "trigger/edge_awaiter.hpp"

namespace corosim {

// --- Primary template ---
template <typename T>
class Signal : private SignalBase {
    template <typename U> friend class Posedge;
    template <typename U> friend class Negedge;
    template <typename U> friend class Change;

public:
    explicit Signal(SignalRegistry& reg, T* ptr = nullptr)
        : ptr_(ptr ? ptr : &store_), prev_val_(ptr ? *ptr : T{}) {
        reg.register_signal(this);
    }

    T read() const { return *ptr_; }
    operator T() const { return read(); }
    void next(T val) { next_val_ = val; pending_ = true; }

private:
    T* ptr_;
    T store_{};
    T prev_val_;
    T next_val_;
    bool pending_ = false;
    bool dirty_   = false;
    bool posedge_flag_ = false;
    bool negedge_flag_ = false;

    void commit() override {
        if (pending_) {
            prev_val_ = *ptr_;
            *ptr_ = next_val_;
            if (!prev_val_ && *ptr_) posedge_flag_ = true;
            if (prev_val_ && !*ptr_) negedge_flag_ = true;
            dirty_ = (prev_val_ != *ptr_);
            pending_ = false;
        }
    }

    bool is_dirty() const override { return dirty_; }
    void clear_dirty() override { dirty_ = false; }
    bool had_posedge() const override { return posedge_flag_; }
    bool had_negedge() const override { return negedge_flag_; }
    void clear_edge_flags() override { posedge_flag_ = false; negedge_flag_ = false; }
};

// --- VlWide<N> specialization (no edge detection) ---
template <int N>
class Signal<VlWide<N>> : private SignalBase {
public:
    explicit Signal(SignalRegistry& reg, VlWide<N>* ptr = nullptr) {
        std::memset(store_.m_storage, 0, sizeof(uint32_t) * N);
        ptr_ = ptr ? ptr : &store_;
        std::memcpy(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N);
        reg.register_signal(this);
    }

    const uint32_t* read() const { return ptr_->m_storage; }
    void next(const VlWide<N>& val) { next_val_ = val; pending_ = true; }

private:
    VlWide<N>* ptr_;
    VlWide<N> store_;
    VlWide<N> prev_val_;
    VlWide<N> next_val_;
    bool pending_ = false;
    bool dirty_   = false;

    void commit() override {
        if (pending_) {
            std::memcpy(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N);
            *ptr_ = next_val_;
            dirty_ = std::memcmp(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N) != 0;
            pending_ = false;
        }
    }

    bool is_dirty() const override { return dirty_; }
    void clear_dirty() override { dirty_ = false; }
    bool had_posedge() const override { return false; }
    bool had_negedge() const override { return false; }
    void clear_edge_flags() override {}
};

// --- bool specialization ---
template <>
class Signal<bool> : private SignalBase {
    template <typename U> friend class Posedge;
    template <typename U> friend class Negedge;
    template <typename U> friend class Change;

public:
    explicit Signal(SignalRegistry& reg, uint8_t* ptr = nullptr)
        : ptr_(ptr ? ptr : &store_), prev_val_(ptr ? *ptr : 0) {
        reg.register_signal(this);
    }

    bool read() const { return *ptr_ != 0; }
    operator bool() const { return read(); }
    void next(bool val) { next_val_ = val ? 1 : 0; pending_ = true; }

private:
    uint8_t* ptr_;
    uint8_t store_{};
    uint8_t prev_val_;
    uint8_t next_val_;
    bool pending_ = false;
    bool dirty_   = false;
    bool posedge_flag_ = false;
    bool negedge_flag_ = false;

    void commit() override {
        if (pending_) {
            prev_val_ = *ptr_;
            *ptr_ = next_val_;
            if (!prev_val_ && *ptr_) posedge_flag_ = true;
            if (prev_val_ && !*ptr_) negedge_flag_ = true;
            dirty_ = (prev_val_ != *ptr_);
            pending_ = false;
        }
    }

    bool is_dirty() const override { return dirty_; }
    void clear_dirty() override { dirty_ = false; }
    bool had_posedge() const override { return posedge_flag_; }
    bool had_negedge() const override { return negedge_flag_; }
    void clear_edge_flags() override { posedge_flag_ = false; negedge_flag_ = false; }
};

// --- Free functions for triggers ---
template <typename T>
Posedge<T> posedge(const Signal<T>& sig) { return Posedge<T>(sig); }

template <typename T>
Negedge<T> negedge(const Signal<T>& sig) { return Negedge<T>(sig); }

template <typename T>
Change<T> change(const Signal<T>& sig) { return Change<T>(sig); }

} // namespace corosim
