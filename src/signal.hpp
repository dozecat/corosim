#pragma once

#include <cstdint>
#include <cstring>
#include <coroutine>
#include <vector>
#include <verilated.h>

#include "types.hpp"

namespace corosim {

class Engine;

namespace detail {

void register_edge_watcher(void* sig_base, TriggerInfo::Type edge, std::coroutine_handle<> h);

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

template <typename T> class Posedge;
template <typename T> class Negedge;
template <typename T> class Change;

} // namespace detail

class SignalRegistry {
public:
    static inline std::vector<detail::SignalBase*> reg;
    static std::vector<detail::SignalBase*>& all() { return reg; }
};

template <typename T>
class Signal : private detail::SignalBase {
    friend class Engine;
    template <typename U> friend class detail::Posedge;
    template <typename U> friend class detail::Negedge;
    template <typename U> friend class detail::Change;

public:
    Signal() : ptr_(&store_), prev_val_(T{}) {
        SignalRegistry::all().push_back(this);
    }

    explicit Signal(T* ptr) : ptr_(ptr), prev_val_(*ptr) {
        SignalRegistry::all().push_back(this);
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
    bool dirty_ = false;
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

template <int N>
class Signal<VlWide<N>> : private detail::SignalBase {
    friend class Engine;

public:
    Signal() {
        std::memset(store_.m_storage, 0, sizeof(uint32_t) * N);
        ptr_ = &store_;
        std::memset(prev_val_.m_storage, 0, sizeof(uint32_t) * N);
        SignalRegistry::all().push_back(this);
    }

    explicit Signal(VlWide<N>* ptr) : ptr_(ptr) {
        std::memcpy(prev_val_.m_storage, ptr->m_storage, sizeof(uint32_t) * N);
        SignalRegistry::all().push_back(this);
    }

    const uint32_t* read() const { return ptr_->m_storage; }
    void next(const VlWide<N>& val) { next_val_ = val; pending_ = true; }

private:
    VlWide<N>* ptr_;
    VlWide<N> store_;
    VlWide<N> prev_val_;
    VlWide<N> next_val_;
    bool pending_ = false;
    bool dirty_ = false;

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

template <>
class Signal<bool> : private detail::SignalBase {
    friend class Engine;
    template <typename U> friend class detail::Posedge;
    template <typename U> friend class detail::Negedge;
    template <typename U> friend class detail::Change;

public:
    Signal() : ptr_(&store_), prev_val_(0) {
        SignalRegistry::all().push_back(this);
    }

    explicit Signal(uint8_t* ptr) : ptr_(ptr), prev_val_(*ptr) {
        SignalRegistry::all().push_back(this);
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
    bool dirty_ = false;
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

namespace detail {

template <typename T>
class Posedge {
    static_assert(sizeof(T) <= 1, "posedge/negedge supports 1-byte signals only");
    friend class ::corosim::Engine;

public:
    explicit Posedge(const Signal<T>& sig) : sig_(const_cast<Signal<T>*>(&sig)) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        register_edge_watcher(static_cast<SignalBase*>(sig_), TriggerInfo::POSEDGE, h);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerInfo::POSEDGE, static_cast<SignalBase*>(sig_), 0};
    }

private:
    Signal<T>* sig_;
};

template <typename T>
class Negedge {
    static_assert(sizeof(T) <= 1, "posedge/negedge supports 1-byte signals only");
    friend class ::corosim::Engine;

public:
    explicit Negedge(const Signal<T>& sig) : sig_(const_cast<Signal<T>*>(&sig)) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        register_edge_watcher(static_cast<SignalBase*>(sig_), TriggerInfo::NEGEDGE, h);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerInfo::NEGEDGE, static_cast<SignalBase*>(sig_), 0};
    }

private:
    Signal<T>* sig_;
};

template <typename T>
class Change {
    friend class ::corosim::Engine;

public:
    explicit Change(const Signal<T>& sig) : sig_(const_cast<Signal<T>*>(&sig)) {}

    bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<> h) {
        register_edge_watcher(static_cast<SignalBase*>(sig_), TriggerInfo::CHANGE, h);
    }

    void await_resume() noexcept {}

    TriggerInfo trigger_info() const {
        return {TriggerInfo::CHANGE, static_cast<SignalBase*>(sig_), 0};
    }

private:
    Signal<T>* sig_;
};

} // namespace detail

template <typename T>
detail::Posedge<T> posedge(const Signal<T>& sig) { return detail::Posedge<T>(sig); }

template <typename T>
detail::Negedge<T> negedge(const Signal<T>& sig) { return detail::Negedge<T>(sig); }

template <typename T>
detail::Change<T> change(const Signal<T>& sig) { return detail::Change<T>(sig); }

} // namespace corosim
