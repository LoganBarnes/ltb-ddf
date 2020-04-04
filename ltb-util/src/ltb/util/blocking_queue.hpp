// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Utilities
// Copyright (c) 2020 Logan Barnes - All Rights Reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
// ///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>

namespace ltb::util {

// Pretty similar to https://stackoverflow.com/a/12805690
template <typename T>
class BlockingQueue {
public:
    void push_back(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value)); // push_back
        }
        condition_.notify_one();
    }

    template <typename... Args>
    void emplace_back(Args&&... args) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            queue_.emplace(std::forward<Args>(args)...); // emplace_back
        }
        condition_.notify_one();
    }

    void clear_all_and_push_back(T value) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            while (not queue_.empty()) {
                queue_.pop();
            }
            queue_.push(std::move(value)); // push_back
        }
        condition_.notify_one();
    }

    template <typename... Args>
    void clear_all_and_emplace_back(Args&&... args) {
        {
            std::lock_guard<std::mutex> lock(mutex_);
            while (not queue_.empty()) {
                queue_.pop();
            }
            queue_.emplace(std::forward<Args>(args)...); // emplace_back
        }
        condition_.notify_one();
    }

    auto pop_front() -> T {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [=] { return !queue_.empty(); });
        T rc(std::move(queue_.front()));
        queue_.pop(); // pop_front
        return rc;
    }

    auto pop_all_but_most_recent() -> T {
        std::lock_guard<std::mutex> scoped_lock(mutex_);
        while (queue_.size() > 1) {
            queue_.pop();
        }
        return queue_.front();
    }

    auto empty() -> bool {
        std::lock_guard<std::mutex> scoped_lock(mutex_);
        return queue_.empty();
    }

    auto size() {
        std::lock_guard<std::mutex> scoped_lock(mutex_);
        return queue_.size();
    }

private:
    std::mutex              mutex_;
    std::condition_variable condition_;
    std::queue<T>           queue_;
};

} // namespace ltb::util
