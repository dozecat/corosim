#pragma once

#include <coroutine>
#include <memory>
#include <optional>
#include <queue>
#include <vector>

#include "core/types.hpp"
#include "coroutine/wait.hpp"
#include "scheduler/fire_ticket.hpp"

namespace corosim {

class TimerEngine {
public:
    using Event = FireTicket;

    void schedule(sim_time deadline, std::coroutine_handle<> h, std::shared_ptr<WaitToken> token,
                  int fire_idx = -1, int* fired = nullptr);

    sim_time now() const { return now_; }

    std::optional<sim_time> next_deadline();

    template <typename Fn>
    void advance_to(sim_time t, Fn&& cb) {
        now_ = t;
        while (!queue_.empty()) {
            auto& top = queue_.top();
            if (!top.ticket.handle || top.ticket.handle.done() ||
                !top.ticket.token || !top.ticket.token->valid()) {
                queue_.pop();
                continue;
            }
            if (top.deadline > now_) {
                break;
            }
            auto entry = std::move(const_cast<Entry&>(top));
            queue_.pop();
            cb(entry.ticket);
        }
    }

private:
    struct Entry {
        sim_time deadline;
        uint64_t timer_id;
        FireTicket ticket;
        bool operator>(const Entry& o) const {
            if (deadline != o.deadline) {
                return deadline > o.deadline;
            }
            return timer_id > o.timer_id;
        }
    };

    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> queue_;
    uint64_t next_id_ = 1;
    sim_time now_ = 0;
};

} // namespace corosim
