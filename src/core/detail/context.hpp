#pragma once

#include <coroutine>
#include <cstddef>
#include "core/types.hpp"

namespace corosim {

class Kernel;
class SignalBase;

namespace detail {

Kernel* find_kernel(std::coroutine_handle<> h);
void map_handle(std::coroutine_handle<> h, Kernel* k);
void unmap_handle(std::coroutine_handle<> h);

void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx = -1, int* fired = nullptr);
void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx = -1, int* fired = nullptr);
void register_compound_wait(std::coroutine_handle<> h, const TriggerInfo* infos, size_t count, int* fired = nullptr);

} // namespace detail

} // namespace corosim
