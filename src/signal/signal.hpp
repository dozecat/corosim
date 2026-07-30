/******************************************************************************
 * Copyright (C) 2025 dozecat. All rights reserved.
 * SPDX-License-Identifier: MIT
 *
 * @file        signal.hpp
 * @brief       Typed simulation signal with non-blocking assignment
 * @see         https://github.com/dozecat/corosim
 *
 * @details     Wraps a Verilator field or software storage; next() schedules
 *              NBA until commit.
 *
 * Modification History:
 * Ver   Who  Date        Changes
 * ----  ---- ----------  -----------------------------------------------------
 * 1.0        2026/07/29  Initial release
 ******************************************************************************/

#pragma once

#include <cstdint>
#include <type_traits>

#include "signal/signal_base.hpp"
#include "signal/signal_registry.hpp"
#include "core/types.hpp"

namespace corosim {

/**
 * @brief Typed signal with read() and non-blocking next().
 * @tparam T Value type (bool is stored as uint8_t).
 *
 * Bind @p ptr to a Verilator field, or omit it for a software-only signal.
 */
template <typename T>
class Signal : public SignalBase {
    static constexpr bool is_bool_alias = std::is_same_v<T, bool>;
    using storage_t = std::conditional_t<is_bool_alias, uint8_t, T>;

public:
    /**
     * @param reg Signal registry that owns commit/edge tracking.
     * @param ptr Optional Verilator/storage pointer; nullptr uses internal store.
     */
    explicit Signal(SignalRegistry& reg, storage_t* ptr = nullptr)
        : reg_(&reg), ptr_(ptr ? ptr : &store_), prev_val_(ptr ? *ptr : storage_t{}) {
        reg_->register_signal(this);
    }

    ~Signal() {
        if (reg_) reg_->unregister_signal(this);
    }

    /** @brief Current committed value. */
    T read() const {
        if constexpr (is_bool_alias) return *ptr_ != 0;
        else return *ptr_;
    }
    operator T() const { return read(); }

    /** @brief Schedule non-blocking assign; applied on next commit_all(). */
    void next(T val) {
        if (!pending_) reg_->mark_pending(this);
        if constexpr (is_bool_alias) next_val_ = val ? 1 : 0;
        else next_val_ = static_cast<storage_t>(val);
        pending_ = true;
    }

    SignalBase* base_ptr() { return this; }

private:
    SignalRegistry* reg_;
    storage_t* ptr_;
    storage_t store_{};
    storage_t prev_val_;
    storage_t next_val_;
    bool pending_ = false;
    bool dirty_   = false;
    bool posedge_flag_ = false;
    bool negedge_flag_ = false;
    bool changed_this_tick_ = false;

    void commit() override {
        if (pending_) {
            prev_val_ = *ptr_;
            *ptr_ = next_val_;
            posedge_flag_ = !prev_val_ && *ptr_;
            negedge_flag_ = prev_val_ && !*ptr_;
            dirty_ = (prev_val_ != *ptr_);
            if (dirty_) changed_this_tick_ = true;
            pending_ = false;
        }
    }

    bool is_dirty() const override { return dirty_; }
    void clear_dirty() override { dirty_ = false; }
    bool has_posedge() const override { return posedge_flag_; }
    bool has_negedge() const override { return negedge_flag_; }
    bool has_changed() const override { return changed_this_tick_; }
    void clear_edge_flags() override { posedge_flag_ = false; negedge_flag_ = false; changed_this_tick_ = false; }
};

} // namespace corosim
