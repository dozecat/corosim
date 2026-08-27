#include "coroutine/coroutine.hpp"

namespace corosim {

void Coroutine::resume() {
    if (state_ == CoroutineState::CANCELLED || state_ == CoroutineState::DONE) return;
    task_.resume();
    if (task_.done()) {
        state_ = CoroutineState::DONE;
    } else {
        state_ = CoroutineState::WAITING;
    }
}

void Coroutine::cancel() {
    state_ = CoroutineState::CANCELLED;
    wait_group_.cancel_all();
}

void Coroutine::restart(Task&& new_task) {
    state_ = CoroutineState::CANCELLED;
    wait_group_.cancel_all();
    task_ = std::move(new_task);
    id_.generation++;
    state_ = CoroutineState::ACTIVE;
}

} // namespace corosim
