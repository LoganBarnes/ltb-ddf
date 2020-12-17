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

#include "example_client.hpp"
#include "example_server.hpp"

auto main(int argc, char* argv[]) -> int {
    std::string host_address = "0.0.0.0:50051";
    auto        client_only  = false;
    auto        server_only  = false;

    auto display_help_message = [] {
        std::cout << "run_example [host_address] [options...]"
                  << "host_address  - Defaults to '0.0.0.0:50051' if not provided\n"
                  << "--client,-c   - Run only the client\n"
                  << "--server,-s   - Run only the server\n"
                  << "--help,-h     - Display this message\n"
                  << std::endl;
    };

    for (auto i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            display_help_message();
        } else if (arg == "--client" || arg == "-c") {
            client_only = true;
        } else if (arg == "--server" || arg == "-s") {
            server_only = true;
        } else {
            host_address = arg;
        }
    }

    // using Service = ltb::example::ChatRoom;

    if (client_only && server_only) {
        std::cerr << "ERROR: the [--client,-c] flag and [--server,-s] flag cannot be used together" << std::endl;
        display_help_message();
        return EXIT_FAILURE;
    }

    if (client_only) {
        ltb::example::ExampleClient client({argc, argv}, host_address);
        return client.exec();
    }

    if (server_only) {
        ltb::example::ExampleServer server(host_address);
        std::cout << "Press enter to exit" << std::endl;
        std::cin.ignore();
        return EXIT_SUCCESS;
    }

    ltb::example::ExampleServer server;
    ltb::example::ExampleClient client({argc, argv}, server.grpc_server());
    return client.exec();
}
