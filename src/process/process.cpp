#include "process.hpp"

namespace corosim {

Process::~Process() {
    wait_group_.cancel_all();
    // Proc destructor handles coroutine cleanup
}

void Process::resume() {
    if (state_ == ProcessState::CANCELLED || state_ == ProcessState::DONE) return;
    task_.resume();
    if (task_.done()) {
        state_ = ProcessState::DONE;
    } else {
        state_ = ProcessState::WAITING;
    }
}

void Process::cancel() {
    state_ = ProcessState::CANCELLED;
    wait_group_.cancel_all();
}

} // namespace corosim
