#pragma once

#include <coroutine>
#include <exception>
#include <functional>

namespace corosim {

class Proc {
public:
    struct promise_type {
        std::coroutine_handle<> continuation;

        Proc get_return_object() noexcept {
            return Proc(std::coroutine_handle<promise_type>::from_promise(*this));
        }

        struct FinalAwaiter {
            bool await_ready() noexcept { return false; }
            void await_suspend(std::coroutine_handle<promise_type> h) noexcept {
                auto& p = h.promise();
                if (p.continuation) p.continuation.resume();
            }
            void await_resume() noexcept {}
        };

        std::suspend_always initial_suspend() noexcept { return {}; }
        FinalAwaiter final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

    Proc(std::coroutine_handle<promise_type> h) noexcept : handle_(h) {}
    Proc(Proc&& other) noexcept : handle_(other.handle_) { other.handle_ = nullptr; }

    Proc& operator=(Proc&& other) noexcept {
        if (this != &other) {
            if (handle_) handle_.destroy();
            handle_ = other.handle_;
            other.handle_ = nullptr;
        }
        return *this;
    }

    ~Proc() { if (handle_) handle_.destroy(); }

    // awaiter interface (for co_await Proc)
    bool await_ready() const noexcept { return done(); }
    void await_suspend(std::coroutine_handle<> h) noexcept {
        handle_.promise().continuation = h;
        if (handle_ && !handle_.done()) handle_.resume();
    }
    void await_resume() noexcept {}

private:
    friend class Engine;
    std::coroutine_handle<promise_type> handle_;
    void resume() { if (handle_ && !handle_.done()) handle_.resume(); }
    bool done() const { return !handle_ || handle_.done(); }
};

namespace detail {
void register_proc(std::function<Proc()> fn);
}

inline void proc(std::function<Proc()> fn) {
    detail::register_proc(std::move(fn));
}

} // namespace corosim
