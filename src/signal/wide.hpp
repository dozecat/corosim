#pragma once

#include <cstring>
#include <verilated.h>

#include "signal/signal.hpp"

namespace corosim {

template <int N>
class Signal<VlWide<N>> : private SignalBase {
public:
    explicit Signal(SignalRegistry& reg, VlWide<N>* ptr = nullptr)
        : reg_(&reg) {
        std::memset(store_.m_storage, 0, sizeof(uint32_t) * N);
        ptr_ = ptr ? ptr : &store_;
        std::memcpy(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N);
        reg_->register_signal(this);
    }

    ~Signal() {
        if (reg_) reg_->unregister_signal(this);
    }

    const uint32_t* read() const { return ptr_->m_storage; }
    void next(const VlWide<N>& val) { if (!pending_) reg_->mark_pending(this); next_val_ = val; pending_ = true; }

    SignalBase* base_ptr() { return this; }

private:
    SignalRegistry* reg_;
    VlWide<N>* ptr_;
    VlWide<N> store_;
    VlWide<N> prev_val_;
    VlWide<N> next_val_;
    bool pending_ = false;
    bool dirty_   = false;
    bool changed_this_tick_ = false;

    void commit() override {
        if (pending_) {
            std::memcpy(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N);
            *ptr_ = next_val_;
            dirty_ = std::memcmp(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N) != 0;
            if (dirty_) changed_this_tick_ = true;
            pending_ = false;
        }
    }

    bool is_dirty() const override { return dirty_; }
    void clear_dirty() override { dirty_ = false; }
    bool had_posedge() const override { return false; }
    bool had_negedge() const override { return false; }
    bool has_changed() const override { return changed_this_tick_; }
    void clear_edge_flags() override { changed_this_tick_ = false; }
};

} // namespace corosim
