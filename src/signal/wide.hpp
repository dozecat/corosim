#pragma once

#include <cstring>

#include <verilated.h>

#include "signal/signal_val.hpp"
#include "signal/signal_registry.hpp"

namespace corosim {

/**
 * @brief Signal specialization for Verilator VlWide<N> vectors.
 * @tparam N Number of 32-bit words in VlWide.
 */
template <int N>
class Signal<VlWide<N>> : public SignalVal {
public:
    explicit Signal(SignalRegistry& reg, VlWide<N>* ptr = nullptr)
        : reg_(&reg) {
        std::memset(store_.m_storage, 0, sizeof(uint32_t) * N);
        ptr_ = ptr ? ptr : &store_;
        std::memcpy(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N);
        reg_->add(this);
    }

    Signal(const Signal&) = delete;
    Signal& operator=(const Signal&) = delete;
    Signal(Signal&&) = delete;
    Signal& operator=(Signal&&) = delete;

    ~Signal() {
        if (reg_) {
            reg_->remove(this);
        }
    }

    /** @brief Pointer to committed wide storage words. */
    const uint32_t* read() const { return ptr_->m_storage; }
    /** @brief Schedule NBA of a full VlWide value. */
    void next(const VlWide<N>& val) {
        if (!pending_) {
            reg_->mark_pending(this);
        }
        next_val_ = val;
        pending_ = true;
    }

    SignalVal* as_val() { return this; }

private:
    SignalRegistry* reg_;
    VlWide<N>* ptr_;
    VlWide<N> store_;
    VlWide<N> prev_val_;
    VlWide<N> next_val_;
    bool pending_ = false;

    void apply_next() override {
        if (pending_) {
            *ptr_ = next_val_;
            pending_ = false;
        }
    }

    TrigInfo trig_info() const override {
        TrigInfo t;
        t.changed = std::memcmp(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N) != 0;
        return t;   // posedge/negedge unsupported for wide vectors
    }

    void update_prev() override {
        std::memcpy(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N);
    }
};

} // namespace corosim
