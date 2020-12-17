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
#include "timer.hpp"

// standard
#include <iostream>

namespace ltb::util {

Timer::Timer(std::string name, std::ostream* os)
    : name_(std::move(name)), ostream_(os), start_time_(std::chrono::steady_clock::now()) {}

auto Timer::start() -> void {
    if (ostream_) {
        (*ostream_) << name_ << "..." << std::endl;
    }
    start_time_ = std::chrono::steady_clock::now();
}

auto Timer::millis_since_start() -> double {
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end_time - start_time_).count();

    if (ostream_) {
        (*ostream_) << name_ << (name_.empty() ? "" : ": ") << std::to_string(duration) << "ms" << std::endl;
    }

    return duration;
}

ScopedTimer::ScopedTimer(std::string name, std::ostream& os) : timer_(std::move(name), &os) {
    timer_.start();
}

ScopedTimer::~ScopedTimer() {
    timer_.millis_since_start();
}

} // namespace ltb::util
