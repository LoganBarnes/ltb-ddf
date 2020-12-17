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
#include "example_service.hpp"

namespace ltb::example {

ExampleService::ExampleService() = default;

auto ExampleService::handle_action(Action const& action, util::Result* result) -> grpc::Status {
    result->mutable_success(); // success by default

    switch (action.action_case()) {

    case Action::kSendMessage: {
        auto const& new_message = action.send_message();

        ChatMessage chat_message = {};
        chat_message.mutable_id()->set_value(std::to_string(chat_message_ids_.size()));
        chat_message.set_value(new_message.message());

        UserMessage user_message           = {};
        *user_message.mutable_client_id()  = new_message.client_id();
        *user_message.mutable_message_id() = chat_message.id();

        chat_message_ids_.emplace_back(chat_message.id().value());
        chat_message_data_.emplace(chat_message.id().value(), chat_message);
        user_message_data_.emplace(user_message.message_id().value(), user_message);
    } break;

    case Action::kChangeClientInfo: {
        result->mutable_error()->set_error_message("Action not yet supported");
    } break;

    case Action::ACTION_NOT_SET: {
        result->mutable_error()->set_error_message("Action value not set properly by client");
    } break;

    } // switch end

    return grpc::Status::OK;
}

} // namespace ltb::example
