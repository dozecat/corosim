#pragma once

#include <coroutine>
#include <memory>
#include <unordered_map>

#include "coroutine/task.hpp"
#include "coroutine/coroutine.hpp"

namespace corosim {

/**
 * @brief Creates and tracks coroutines.
 *
 * Decoupled from Kernel: spawn() only creates (does NOT resume);
 * the caller binds promise context and resumes.
 */
class CoroutineManager {
public:
    template <typename Fn>
    Coroutine* spawn(Fn&& fn) {
        auto holder = std::make_shared<std::decay_t<Fn>>(std::forward<Fn>(fn));
        auto task = (*holder)();
        auto id = CoroutineId{next_id_++, 0};
        auto proc = std::make_unique<Coroutine>(id, std::move(task));
        proc->set_factory_holder(std::move(holder));
        auto* raw = proc.get();
        coroutines_[id.id] = std::move(proc);
        return raw;
    }

    /**
     * @brief Replace the body of coroutine @p id with a fresh coroutine.
     * @return The coroutine pointer (same memory, bumped generation). Does not resume.
     */
    template <typename Fn>
    Coroutine* restart(CoroutineId id, Fn&& fn) {
        auto it = coroutines_.find(id.id);
        if (it == coroutines_.end() || !it->second) return nullptr;
        auto* proc = it->second.get();
        auto holder = std::make_shared<std::decay_t<Fn>>(std::forward<Fn>(fn));
        auto new_task = (*holder)();
        proc->restart(std::move(new_task));
        proc->set_factory_holder(std::move(holder));
        return proc;
    }

    Coroutine* find_coroutine(std::coroutine_handle<> h) const;
    size_t active_count() const;
    void cleanup_finished();
    std::exception_ptr collect_exceptions();

private:
    uint64_t next_id_ = 1;
    std::unordered_map<uint64_t, std::unique_ptr<Coroutine>> coroutines_;
};

} // namespace corosim
