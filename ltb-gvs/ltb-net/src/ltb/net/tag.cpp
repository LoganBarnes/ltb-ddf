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
#include "tag.hpp"

namespace ltb::net {

ClientTag::ClientTag(void* d, ClientTagLabel l) : data(d), label(l) {}

ServerTag::ServerTag(void* d, ServerTagLabel l) : data(d), label(l) {}

std::ostream& operator<<(std::ostream& os, const ClientTag& tag) {
    os << '{' << tag.data << ", ";

    switch (tag.label) {
    case ClientTagLabel::ConnectionChange:
        os << "ClientTagLabel::ConnectionChange";
        break;
    case ClientTagLabel::UnaryFinished:
        os << "ClientTagLabel::UnaryFinished";
        break;
    }
    return os << '}';
}

std::ostream& operator<<(std::ostream& os, const ServerTag& tag) {
    os << '{' << tag.data << ", ";

    switch (tag.label) {
    case ServerTagLabel::NewRpc:
        os << "ServerTagLabel::NewRpc";
        break;
    case ServerTagLabel::Done:
        os << "ServerTagLabel::Done";
        break;
    case ServerTagLabel::Writing:
        os << "ServerTagLabel::Writing";
        break;
    }
    return os << '}';
}

namespace detail {

void* make_tag(void* data, ClientTagLabel label, std::unordered_map<void*, std::unique_ptr<ClientTag>>* tags) {
    auto&& tag    = std::make_unique<ClientTag>(data, label);
    void*  result = tag.get();
    tags->emplace(result, std::forward<decltype(tag)>(tag));
    return result;
}

void* make_tag(void* data, ServerTagLabel label, std::unordered_map<void*, std::unique_ptr<ServerTag>>* tags) {
    auto&& tag    = std::make_unique<ServerTag>(data, label);
    void*  result = tag.get();
    tags->emplace(result, std::forward<decltype(tag)>(tag));
    return result;
}

ClientTag get_tag(void* key, std::unordered_map<void*, std::unique_ptr<ClientTag>>* tags) {
    if (tags->find(key) == tags->end()) {
        throw std::runtime_error("provided client tag does not exist in the map");
    }
    ClientTag tag_copy = *tags->at(key);
    tags->erase(key);
    return tag_copy;
}

ServerTag get_tag(void* key, std::unordered_map<void*, std::unique_ptr<ServerTag>>* tags) {
    if (tags->find(key) == tags->end()) {
        throw std::runtime_error("provided server tag does not exist in the map");
    }
    ServerTag tag_copy = *tags->at(key);
    tags->erase(key);
    return tag_copy;
}

} // namespace detail
} // namespace ltb::net
