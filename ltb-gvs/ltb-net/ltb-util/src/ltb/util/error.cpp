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
#include "error.hpp"

// generated
#include <ltb/paths.hpp>

// external
#include <doctest/doctest.h>

namespace ltb::util {

SourceLocation::SourceLocation(std::string file, int line) : filename(std::move(file)), line_number(line) {}

namespace {

///
/// \brief Get the file string relative to the project root dir.
///
auto get_relative_path_string(const std::string& path) -> std::string {
    auto abs_path_start = path.find(paths::project_root());

    // Remove the project path if it exists in `path`
    if (abs_path_start == 0) {
        return path.substr(paths::project_root().size());
    }

    return path;
}

TEST_CASE("[ltb][util] get relative path string") {
    CHECK(get_relative_path_string("/no/project/root") == "/no/project/root");
    CHECK(get_relative_path_string(paths::project_root() + "with/project/root") == "with/project/root");
}

///
/// \brief Prepend the file and line number to the message if they were specified.
///
auto create_debug_message(const SourceLocation& source_location, const std::string& error_message) -> std::string {
    std::string result = "[";

    if (!source_location.filename.empty()) {
        result += get_relative_path_string(source_location.filename);
    }

    if (source_location.line_number >= 0) {
        result += ':' + std::to_string(source_location.line_number);
    }

    result += "] " + error_message;
    return result;
}

} // namespace

Error::~Error() = default;

Error::Error(SourceLocation source_location, Severity severity, std::string error_message)
    : source_location_(std::move(source_location)),
      severity_(severity),
      error_message_(std::move(error_message)),
      debug_message_(create_debug_message(source_location_, error_message_)) {}

auto Error::severity() const -> Error::Severity const& {
    return severity_;
}

auto Error::error_message() const -> std::string const& {
    return error_message_;
}

auto Error::debug_error_message() const -> std::string const& {
    return debug_message_;
}

auto Error::append_message(const Error& error, const std::string& message) -> Error {
    return Error(error.source_location_, error.severity_, error.error_message_ + " " + message);
}

auto Error::operator==(const Error& other) const -> bool {
    return debug_message_ == other.debug_message_;
}

auto Error::operator!=(const Error& other) const -> bool {
    return !(this->operator==(other));
}

} // namespace ltb::util

#include <iostream>

namespace {

[[maybe_unused]] auto build_expected_message(std::string const& file_prefix,
                                             int const&         line_number,
                                             std::string const& error_message) -> std::string {

    // TODO: use std::filesystem when it is available on most compilers
    using namespace ltb::paths;
    auto prefix    = (file_prefix.empty() ? std::string() : file_prefix + slash());
    auto this_file = "src" + slash() + "ltb" + slash() + "util" + slash() + "error.cpp";
    return "[" + prefix + this_file + ":" + std::to_string(line_number) + "] " + error_message;
}

} // namespace

TEST_CASE("[ltb][util] check error helpers") {
    CHECK(LTB_MAKE_ERROR("Error message").error_message() == "Error message");
    CHECK(LTB_MAKE_ERROR("blarg") == LTB_MAKE_ERROR("blarg"));
    CHECK(LTB_MAKE_ERROR("this is bad").severity() == ltb::util::Error::Severity::Error);
}

TEST_CASE("[ltb][util] check warning helpers") {
    CHECK(LTB_MAKE_WARNING("Warning message").error_message() == "Warning message");
    CHECK(LTB_MAKE_WARNING("blarg") == LTB_MAKE_WARNING("blarg"));
    CHECK(LTB_MAKE_WARNING("not so bad").severity() == ltb::util::Error::Severity::Warning);
}
