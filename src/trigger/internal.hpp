#pragma once

#include <coroutine>
#include "../core/types.hpp"

namespace corosim {

class SignalBase;

namespace detail {

void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h);
void register_delay_wait(std::coroutine_handle<> h, sim_time interval);
void register_compound_wait(std::coroutine_handle<> h, const TriggerInfo* infos, size_t count);

} // namespace detail

} // namespace corosim
