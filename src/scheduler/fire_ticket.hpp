#pragma once

#include <coroutine>
#include <memory>

#include "coroutine/wait.hpp"

namespace corosim {

/** @brief Ticket to fire a suspended coroutine (shared by timer and monitor). */
struct FireTicket {
    std::coroutine_handle<> handle;
    std::shared_ptr<WaitToken> token;
    int* fire_idx = nullptr;    // where to write the winning trigger index
    int fire_value = -1;        // winning trigger index
};

} // namespace corosim
