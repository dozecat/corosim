#pragma once

#include <cstdint>

namespace corosim {

/** @brief The trigger conditions a signal currently satisfies (query result). */
struct TrigInfo {
    bool posedge = false;   // bit0: 0→1
    bool negedge = false;   // bit0: 1→0
    bool changed = false;   // value changed vs prev_val
};

/**
 * @brief Signal value abstraction (current / pending / previous value).
 *
 * Write/observe separation:
 *   - apply_next()      commit a pending NBA (pure write, no detection)
 *   - trig_info()       pure query of current value vs prev_val
 *   - update_prev()     advance prev_val <- current (once per tick)
 */
class SignalVal {
public:
    virtual ~SignalVal() = default;
    virtual void apply_next() = 0;
    virtual TrigInfo trig_info() const = 0;
    virtual void update_prev() = 0;
};

} // namespace corosim
