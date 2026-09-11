#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "trigger/spec.hpp"
#include "signal/signal_val.hpp"

namespace corosim {

/**
 * @brief Active wait token, shared between a WaitGroup and scheduler queues.
 *
 * Lifetime is managed by shared_ptr so scheduler entries never dangle even if
 * the WaitGroup entry is reaped or the coroutine is destroyed.
 */
class WaitToken {
public:
    bool valid() const { return valid_; }
    void invalidate() { valid_ = false; }

private:
    bool valid_ = true;
};

/** @brief Tracks a coroutine's outstanding waits (no dependency on CoroutineId). */
class WaitGroup {
public:
    WaitGroup() = default;

    std::shared_ptr<WaitToken> add_trigger_watch(SignalVal* sig, TriggerType type);
    std::shared_ptr<WaitToken> add_delay_watch();

    void cancel_all();
    void cancel_others(const std::shared_ptr<WaitToken>& keep);   // any() sibling cancel

private:
    std::vector<std::shared_ptr<WaitToken>> entries_;
    void cleanup_invalid();   // remove invalid entries; shared_ptr copies stay alive
};

} // namespace corosim
