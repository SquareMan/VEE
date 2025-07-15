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


#include "Logging.hpp"
#include "LogUtil.hpp"

#include <chrono>
#include <iostream>


void vee::_log_impl(log::Severity severity, std::string_view msg, const std::source_location& location) {
    using namespace vee::log;
    auto time = ColoredExpr(std::format("[{:%H:%M:%S}]", std::chrono::system_clock::now()), Color::Faint);
    auto colored_severity = ColoredExpr(severity, severity >= Severity::Error ? Color::Red : Color::Faint);

    // Make sure that we only add our own newline if the message doesn't already end with one.
    const char* auto_newline = msg.ends_with('\n') ? "" : "\n";
    auto full_msg = std::format(": {}{}", msg, auto_newline);

    switch (severity) {
    case Severity::Trace:
    case Severity::Debug:
    case Severity::Info:
    case Severity::Warning:
        std::print(std::clog, "{} {} {}", time, colored_severity, ColoredExpr(full_msg, Color::Faint));
        break;
    case Severity::Error:
    case Severity::Fatal:
        auto colored_loc = ColoredExpr(
            std::format("{}:{} ({})", location.file_name(), location.line(), LastN(location.function_name(), 30)), Color::Cyan
        );
        std::print(std::clog, "{} {} {} {}", time, colored_severity, colored_loc, ColoredExpr(full_msg, Color::Red));
        break;
    }
}