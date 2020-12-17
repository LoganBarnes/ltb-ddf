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
// project
#include "ltb/gvs/core/nil_scene.hpp"
#include "ltb/gvs/net/client_scene.hpp"

// examples
#include "../common/test_scene.hpp"

int main(int argc, char* argv[]) {
    std::string server_address = "0.0.0.0:50055";

    if (argc > 1) {
        server_address = argv[1];
    }

    std::unique_ptr<gvs::Scene> scene;

    {
        auto client_scene = std::make_unique<gvs::net::ClientScene>(server_address);
        if (client_scene->connected()) {
            scene = std::move(client_scene);
        } else {
            scene = std::make_unique<gvs::NilScene>();
        }
    }

    scene->clear();
    example::build_test_scene(scene.get());

    return 0;
}
