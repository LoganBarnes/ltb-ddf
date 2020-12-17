// ///////////////////////////////////////////////////////////////////////////////////////
// LTB Geometry Visualization Server
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
#include "imgui_colors.hpp"

namespace ltb::gvs {

auto red() -> ImVec4 {
    return {1.f, 0.f, 0.f, 1.f};
}
auto green() -> ImVec4 {
    return {0.f, 1.f, 0.f, 1.f};
}
auto blue() -> ImVec4 {
    return {0.f, 0.f, 1.f, 1.f};
}
auto yellow() -> ImVec4 {
    return {1.f, 1.f, 0.f, 1.f};
}
auto cyan() -> ImVec4 {
    return {0.f, 1.f, 1.f, 1.f};
}
auto magenta() -> ImVec4 {
    return {1.f, 0.f, 1.f, 1.f};
}
auto orange() -> ImVec4 {
    return {1.f, 0.5f, 0.1f, 1.f};
}
auto black() -> ImVec4 {
    return {0.f, 0.f, 0.f, 1.f};
}
auto white() -> ImVec4 {
    return {1.f, 1.f, 1.f, 1.f};
}
auto gray() -> ImVec4 {
    return {0.5f, 0.5f, 0.5f, 1.f};
}
auto dark_gray() -> ImVec4 {
    return {0.35f, 0.35f, 0.35f, 1.f};
}
auto light_gray() -> ImVec4 {
    return {0.75f, 0.75f, 0.75f, 1.f};
}

} // namespace ltb::gvs
