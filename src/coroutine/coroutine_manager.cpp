#include "coroutine/coroutine_manager.hpp"
#include "coroutine/coroutine.hpp"

namespace corosim {

void CoroutineManager::cleanup_finished() {
    auto it = coroutines_.begin();
    while (it != coroutines_.end()) {
        if (!it->second || it->second->done()) {
            it = coroutines_.erase(it);
        } else {
            ++it;
        }
    }
}

std::exception_ptr CoroutineManager::collect_exceptions() {
    for (auto& [id, coro] : coroutines_) {
        if (coro && coro->done()) {
            auto ep = coro->get_exception();
            if (ep) {
                return ep;
            }
        }
    }
    return nullptr;
}

} // namespace corosim
