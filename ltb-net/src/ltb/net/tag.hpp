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
#pragma once

// standard
#include <memory>
#include <ostream>
#include <unordered_map>

namespace ltb::net {

enum class ClientTagLabel {
    ConnectionChange,
    UnaryFinished,
};

enum class ServerTagLabel {
    NewRpc,
    Writing,
    Done,
};

struct ClientTag {
    void*          data;
    ClientTagLabel label;

    ClientTag(void* d, ClientTagLabel l);
};

struct ServerTag {
    void*          data;
    ServerTagLabel label;

    ServerTag(void* d, ServerTagLabel l);
};

std::ostream& operator<<(std::ostream& os, const ClientTag& tag);
std::ostream& operator<<(std::ostream& os, const ServerTag& tag);

namespace detail {

void* make_tag(void* data, ClientTagLabel label, std::unordered_map<void*, std::unique_ptr<ClientTag>>* tags);
void* make_tag(void* data, ServerTagLabel label, std::unordered_map<void*, std::unique_ptr<ServerTag>>* tags);

ClientTag get_tag(void* key, std::unordered_map<void*, std::unique_ptr<ClientTag>>* tags);
ServerTag get_tag(void* key, std::unordered_map<void*, std::unique_ptr<ServerTag>>* tags);

} // namespace detail
} // namespace ltb::net
