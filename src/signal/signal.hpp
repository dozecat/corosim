#pragma once

#include <cstdint>
#include <type_traits>

#include "signal/signal_val.hpp"
#include "signal/signal_registry.hpp"

namespace corosim {

/**
 * @brief Typed signal with read() and non-blocking next().
 * @tparam T Value type (bool is stored as uint8_t).
 */
template <typename T>
class Signal : public SignalVal {
    static constexpr bool is_bool_alias = std::is_same_v<T, bool>;
    using storage_t = std::conditional_t<is_bool_alias, uint8_t, T>;

public:
    /**
     * @param reg Signal registry that tracks membership/pending.
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

    /** @brief Schedule non-blocking assign; applied on next apply_pending(). */
    void next(T val) {
        if (!pending_) reg_->mark_pending(this);
        if constexpr (is_bool_alias) next_val_ = val ? 1 : 0;
        else next_val_ = static_cast<storage_t>(val);
        pending_ = true;
    }

    SignalVal* as_val() { return this; }

private:
    SignalRegistry* reg_;
    storage_t* ptr_;
    storage_t store_{};
    storage_t prev_val_;
    storage_t next_val_;
    bool pending_ = false;

    void apply_next() override {
        if (pending_) {
            *ptr_ = next_val_;
            pending_ = false;
        }
    }

    TrigInfo trig_info() const override {
        storage_t cur = *ptr_;
        TrigInfo t;
        t.posedge = (prev_val_ == 0) && (cur != 0);
        t.negedge = (prev_val_ != 0) && (cur == 0);
        t.changed = (prev_val_ != cur);
        return t;
    }

    void update_prev() override {
        prev_val_ = *ptr_;
    }
};

} // namespace corosim
