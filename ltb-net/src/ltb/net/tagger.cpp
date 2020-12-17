// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Networking
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
#include "tagger.hpp"

namespace ltb::net {

auto ClientTagger::make_tag(void* data, ClientTagLabel label) -> void* {
    std::lock_guard<std::mutex> lock(mutex_);
    return detail::make_tag(data, label, &tags_);
}

auto ClientTagger::get_tag(void* key) -> ClientTag {
    std::lock_guard<std::mutex> lock(mutex_);
    return detail::get_tag(key, &tags_);
}

auto ServerTagger::make_tag(void* data, ServerTagLabel label) -> void* {
    std::lock_guard<std::mutex> lock(mutex_);
    return detail::make_tag(data, label, &tags_);
}

auto ServerTagger::get_tag(void* key) -> ServerTag {
    std::lock_guard<std::mutex> lock(mutex_);
    return detail::get_tag(key, &tags_);
}

} // namespace ltb::net
