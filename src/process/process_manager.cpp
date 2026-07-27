#include "process_manager.hpp"

namespace corosim {

void ProcessManager::init_all() {
    for (auto& [id, proc] : processes_) {
        if (proc && proc->active() && !proc->done()) {
            handle_map_[proc->void_handle().address()] = proc.get();
        }
    }
}

Process* ProcessManager::find_process(std::coroutine_handle<> h) const {
    auto it = handle_map_.find(h.address());
    return (it != handle_map_.end()) ? it->second : nullptr;
}

size_t ProcessManager::active_count() const {
    size_t n = 0;
    for (auto& [id, proc] : processes_) {
        if (proc && proc->active() && !proc->done()) n++;
    }
    return n;
}

void ProcessManager::cleanup_finished() {
    auto it = processes_.begin();
    while (it != processes_.end()) {
        if (!it->second || it->second->done()) {
            if (it->second && !it->second->done()) {
                handle_map_.erase(it->second->void_handle().address());
            }
            it = processes_.erase(it);
        } else {
            ++it;
        }
    }
}

} // namespace corosim
