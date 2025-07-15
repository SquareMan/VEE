//    Copyright 2025 Steven Casper
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.


#include "Assert.hpp"

#include "LogUtil.hpp"

#include <chrono>
#include <format>
#include <iostream>

void vee::assert::detail::_log_assert(const char* expr_str, const std::source_location& location, std::string_view msg) {
    using namespace vee::log;
    auto time = ColoredExpr(std::format("[{:%H:%M:%S}]", std::chrono::system_clock::now()), Color::Faint);
    auto assertion = ColoredExpr(std::format("ASSERT: {}", expr_str), Color::Red);
    auto colored_loc = ColoredExpr(
        std::format("{}:{} ({})", location.file_name(), location.line(), LastN(location.function_name(), 30)), Color::Cyan
    );

    // Make sure that we only add our own newline if the message doesn't already end with one.
    const char* auto_newline = msg.ends_with('\n') ? "" : "\n";
    auto full_msg = std::format(": {}{}", msg, auto_newline);
    std::print(std::clog, "{} {}  {}\n\tMessage:\t{}", time, assertion, colored_loc, full_msg);
}