#include "context.hpp"
#include "core/kernel.hpp"

#include <cassert>
#include <unordered_map>

namespace corosim {

namespace {
std::unordered_map<void*, Kernel*>& kernel_handle_map() {
    static std::unordered_map<void*, Kernel*> map;
    return map;
}
} // anonymous namespace

namespace detail {

Kernel* find_kernel(std::coroutine_handle<> h) {
    auto it = kernel_handle_map().find(h.address());
    return it != kernel_handle_map().end() ? it->second : nullptr;
}

void map_handle(std::coroutine_handle<> h, Kernel* k) {
    kernel_handle_map()[h.address()] = k;
}

void unmap_handle(std::coroutine_handle<> h) {
    kernel_handle_map().erase(h.address());
}

void register_edge_wait(SignalBase* sig, TriggerType edge, std::coroutine_handle<> h, int fire_idx, int* fired) {
    auto* k = find_kernel(h);
    if (!k) return;
    k->register_edge_wait(sig, edge, h, fire_idx, fired);
}

void register_delay_wait(std::coroutine_handle<> h, sim_time interval, int fire_idx, int* fired) {
    auto* k = find_kernel(h);
    if (!k) return;
    k->register_delay_wait(h, interval, fire_idx, fired);
}

void register_compound_wait(std::coroutine_handle<> h, const TriggerInfo* infos, size_t count, int* fired) {
    auto* k = find_kernel(h);
    if (!k) return;
    for (size_t i = 0; i < count; ++i) {
        switch (infos[i].type) {
        case TriggerInfo::POSEDGE:
        case TriggerInfo::NEGEDGE:
        case TriggerInfo::CHANGE:
            k->register_edge_wait(infos[i].sig, infos[i].type, h, (int)i, fired);
            break;
        case TriggerInfo::DELAY:
            k->register_delay_wait(h, infos[i].interval, (int)i, fired);
            break;
        }
    }
}

} // namespace detail

} // namespace corosim
