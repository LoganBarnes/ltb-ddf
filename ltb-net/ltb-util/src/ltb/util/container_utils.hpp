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

#include <algorithm>
#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace ltb::util {

template <typename Map, typename Key>
auto has_key(const Map& map, const Key& key) -> bool {
    return map.find(key) != map.end();
}

template <typename V, typename T>
auto has_item(V& v, const T& item) {
    return std::find(std::begin(v), std::end(v), item) != std::end(v);
}

template <typename V, typename T>
auto remove_all_by_value(V& v, const T& value) {
    return v.erase(std::remove(std::begin(v), std::end(v), value), std::end(v));
}

template <typename V, typename Pred>
auto remove_all_by_predicate(V& v, Pred&& pred) {
    return v.erase(std::remove_if(std::begin(v), std::end(v), std::forward<Pred>(pred)), std::end(v));
}

} // namespace ltb::util
