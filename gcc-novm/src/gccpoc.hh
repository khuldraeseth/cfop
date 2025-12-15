#ifndef _GCC_BASHTEST_H_
#define _GCC_BASHTEST_H_

#include <coroutine>
#include <utility>

class task {
public:
    class promise_type {
    public:
        auto get_return_object() noexcept -> task;

        auto initial_suspend() noexcept -> std::suspend_always;

        auto return_void() noexcept -> void;

        auto unhandled_exception() noexcept -> void;

        struct final_awaiter {
            auto await_ready() noexcept -> bool;
            auto await_suspend(std::coroutine_handle<promise_type> h) noexcept -> std::coroutine_handle<>;
            auto await_resume() noexcept -> void;
        };

        auto final_suspend() noexcept -> final_awaiter;

        std::coroutine_handle<> continuation;
    };

    task(task const&) = delete;
    auto operator=(task const&) = delete;

    auto operator=(task&&) = delete;
    task(task&& t) noexcept
        : coro_(std::exchange(t.coro_, {})) {}

    ~task() {
        if (coro_) {
            coro_.destroy();
        }
    }

    auto start() noexcept -> void;
    auto finalize() noexcept -> void;

    class awaiter {
    public:
        auto await_ready() noexcept -> bool;

        auto await_suspend(std::coroutine_handle<> continuation) noexcept -> std::coroutine_handle<>;

        auto await_resume() noexcept -> void;

    private:
        friend task;
        explicit awaiter(std::coroutine_handle<task::promise_type> h) noexcept
            : coro_(h) {}

        std::coroutine_handle<task::promise_type> coro_;
    };

    auto operator co_await() && noexcept -> awaiter { return awaiter { coro_ }; }

private:
    explicit task(std::coroutine_handle<promise_type> h) noexcept
        : coro_(h) {}

    std::coroutine_handle<promise_type> coro_;
};


#endif
