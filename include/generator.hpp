//
// Created by define on 19.06.2026.
//

#ifndef SOSAL_GENERATOR_HPP
#define SOSAL_GENERATOR_HPP

#include <coroutine>
#include <exception>

#include "generator.hpp"

template <typename T>
class Generator {
public:
    struct promise_type;
    using coro_handle_t = std::coroutine_handle<promise_type>;

    struct promise_type {
        const T* value = nullptr;

        auto get_return_object() {
            return coro_handle_t::from_promise(*this);
        }

        auto initial_suspend() {
            return std::suspend_always{};
        }

        auto final_suspend() noexcept {
            return std::suspend_always{};
        }

        auto yield_value(const T& val) {
            value = std::addressof(val);
            return std::suspend_always{};
        }

        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };

private:
    coro_handle_t handle_;

public:
    Generator(coro_handle_t handle) : handle_(handle) {}
    ~Generator() { if (handle_) handle_.destroy(); }

    Generator(const Generator&) = delete;
    Generator& operator=(const Generator&) = delete;

    Generator(Generator&& rhs) noexcept
        : handle_(rhs)
    {
        rhs.handle_ = std::coroutine_handle<>{};
    }

    Generator& operator=(Generator&& rhs) noexcept {
        if (this != &rhs) {
            if (handle_) {
                handle_.destroy();
            }

            handle_ = rhs.handle_;
            rhs.handle_ = std::coroutine_handle<>{};
        }

        return *this;
    }

    bool Next() {
        if (!handle_.done()) {
            handle_.resume();
        }

        return !handle_.done();
    }

    T Value() {
        return *handle_.promise().value;
    }

    // iterator
public:
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using reference = T&;

        explicit iterator(coro_handle_t h) : handle_(h) {}

        T const& operator*() {
            return *handle_.promise().value;
        }

        T const* operator->() {
            return handle_.promise().value;
        }

        iterator& operator++() {
            handle_.resume();
            if (handle_.done())
                handle_ = nullptr;
            return *this;
        }

        auto operator<=>(const iterator&) const = default;

    private:
        coro_handle_t handle_;
    };

    iterator begin() {
        if (!handle_)
            return iterator{nullptr};
        handle_.resume();
        if (handle_.done())
            return iterator{nullptr};
        return iterator{handle_};
    }

    iterator end() {
        return iterator{nullptr};
    }
};

#endif //SOSAL_GENERATOR_HPP
