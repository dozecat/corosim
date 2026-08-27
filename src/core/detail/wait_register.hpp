#pragma once

#include <coroutine>
#include <cstddef>

#include "core/types.hpp"
#include "trigger/spec.hpp"

namespace corosim {

class Kernel;
class SignalVal;

namespace detail {

Kernel* get_kernel(std::coroutine_handle<> h);

void register_trigger_wait(SignalVal* sig, TriggerType t, std::coroutine_handle<> h,
                           int fire_idx = -1, int* fired = nullptr);
void register_delay_wait(std::coroutine_handle<> h, sim_time interval,
                         int fire_idx = -1, int* fired = nullptr);
void register_compound_wait(std::coroutine_handle<> h, const Trigger* infos, size_t count,
                            int* fired = nullptr);

} // namespace detail

} // namespace corosim
