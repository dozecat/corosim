/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        wide.hpp
 * @brief       Signal specialization for Verilator VlWide<N>
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Supports wide HDL vectors via VlWide storage and change-only
 *              edge flags.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <cstring>
#include <verilated.h>

#include "signal/signal.hpp"

namespace corosim {

/**
 * @brief Signal specialization for Verilator VlWide<N> vectors.
 * @tparam N Number of 32-bit words in VlWide.
 */
template <int N>
class Signal<VlWide<N>> : public SignalBase {
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

    /** @brief Pointer to committed wide storage words. */
    const uint32_t* read() const { return ptr_->m_storage; }
    /** @brief Schedule NBA of a full VlWide value. */
    void next(const VlWide<N>& val) { if (!pending_) reg_->mark_pending(this); next_val_ = val; pending_ = true; }

    SignalBase* base_ptr() { return this; }

private:
    SignalRegistry* reg_;
    VlWide<N>* ptr_;
    VlWide<N> store_;
    VlWide<N> prev_val_;
    VlWide<N> next_val_;
    bool pending_ = false;
    bool marked_changed_ = false;
    bool changed_this_tick_ = false;

    void commit() override {
        if (pending_) {
            std::memcpy(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N);
            *ptr_ = next_val_;
            if (std::memcmp(prev_val_.m_storage, ptr_->m_storage, sizeof(uint32_t) * N) != 0) {
                changed_this_tick_ = true;
                if (reg_) reg_->mark_changed(this);
            }
            pending_ = false;
        }
    }

    bool has_posedge() const override { return false; }
    bool has_negedge() const override { return false; }
    bool has_changed() const override { return changed_this_tick_; }
    void clear_edge_flags() override { changed_this_tick_ = false; }
    bool marked_changed() const override { return marked_changed_; }
    void set_marked_changed(bool v) override { marked_changed_ = v; }
};

} // namespace corosim
