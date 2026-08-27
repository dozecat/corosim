#include "coroutine/coroutine_manager.hpp"
#include "coroutine/coroutine.hpp"

namespace corosim {

Coroutine* CoroutineManager::find_coroutine(std::coroutine_handle<> h) const {
    return Task::promise_type::from_handle(h).coroutine;
}

size_t CoroutineManager::active_count() const {
    size_t n = 0;
    for (auto& [id, proc] : coroutines_) {
        if (proc && proc->active() && !proc->done()) n++;
    }
    return n;
}

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
    for (auto& [id, proc] : coroutines_) {
        if (proc && proc->done()) {
            auto ep = proc->get_exception();
            if (ep) return ep;
        }
    }
    return nullptr;
}

} // namespace corosim
