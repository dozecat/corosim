#include "core/detail/wait_register.hpp"

#include <type_traits>
#include <variant>

#include "core/kernel.hpp"
#include "coroutine/task.hpp"

namespace corosim {

namespace detail {

Kernel* get_kernel(std::coroutine_handle<> h) {
    return Task::promise_type::from_handle(h).kernel;
}

void register_trigger_wait(SignalVal* sig, TriggerType t, std::coroutine_handle<> h,
                           int fire_idx, int* fired) {
    auto* k = get_kernel(h);
    if (!k) {
        return;
    }
    k->register_trigger_wait(sig, t, h, fire_idx, fired);
}

void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx, int* fired) {
    auto* k = get_kernel(h);
    if (!k) {
        return;
    }
    k->register_delay_wait(h, interval, fire_idx, fired);
}

/**
 * @brief Register every trigger in @p infos against the same coroutine.
 * @param fired Written with the winning trigger index when one fires.
 */
void register_compound_wait(std::coroutine_handle<> h, const Trigger* infos, size_t count, int* fired) {
    auto* k = get_kernel(h);
    if (!k) {
        return;
    }
    for (size_t i = 0; i < count; ++i) {
        std::visit([&](const auto& spec) {
            using T = std::decay_t<decltype(spec)>;
            if constexpr (std::is_same_v<T, DelaySpec>) {
                k->register_delay_wait(h, spec.interval, (int)i, fired);
            } else {
                k->register_trigger_wait(spec.sig, spec.type, h, (int)i, fired);
            }
        }, infos[i]);
    }
}

} // namespace detail

} // namespace corosim
