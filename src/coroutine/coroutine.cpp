#include "coroutine/coroutine.hpp"

namespace corosim {

void Coroutine::resume() {
    if (state_ == CoroutineState::CANCELLED || state_ == CoroutineState::DONE) {
        return;
    }
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

} // namespace corosim
