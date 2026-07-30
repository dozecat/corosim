#pragma once

#include <coroutine>
#include <optional>
#include <queue>
#include <vector>
#include "core/types.hpp"

namespace corosim {

class TimerEngine {
public:
    struct Event {
        std::coroutine_handle<> handle;
        WaitId* wid = nullptr;
        int* fire_idx = nullptr;
        int fire_value = -1;
    };

    TimerId schedule(sim_time deadline, std::coroutine_handle<> h, WaitId* wid,
                     int fire_idx = -1, int* fired = nullptr);

    sim_time now() const { return now_; }

    std::optional<sim_time> next_deadline();

    template <typename Fn>
    void advance_to(sim_time t, Fn&& cb) {
        now_ = t;
        while (!queue_.empty()) {
            auto& top = queue_.top();
            if (!top.handle || top.handle.done() || !top.wid || !top.wid->valid()) {
                queue_.pop();
                continue;
            }
            if (top.deadline > now_) break;
            auto entry = std::move(const_cast<Entry&>(top));
            queue_.pop();
            cb({entry.handle, entry.wid, entry.fire_idx, entry.fire_value});
        }
    }

private:
    struct Entry {
        sim_time deadline;
        TimerId timer_id;
        std::coroutine_handle<> handle;
        WaitId* wid = nullptr;
        int* fire_idx = nullptr;
        int fire_value = -1;
        bool operator>(const Entry& o) const { return deadline > o.deadline; }
    };

    std::priority_queue<Entry, std::vector<Entry>, std::greater<>> queue_;
    uint64_t next_id_ = 1;
    sim_time now_ = 0;
};

} // namespace corosim
