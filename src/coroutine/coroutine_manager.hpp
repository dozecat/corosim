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
        auto id = CoroutineId{next_id_++};
        auto coro = std::make_unique<Coroutine>(id, std::move(task));
        coro->set_factory_holder(std::move(holder));
        auto* raw = coro.get();
        coroutines_[id.id] = std::move(coro);
        return raw;
    }

    void cleanup_finished();
    std::exception_ptr collect_exceptions();

private:
    uint64_t next_id_ = 1;
    std::unordered_map<uint64_t, std::unique_ptr<Coroutine>> coroutines_;
};

} // namespace corosim
