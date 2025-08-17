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

static vee::Color get_severity_color(vee::LogSeverity severity) {
    switch (severity) {
    default:
    case vee::LogSeverity::Trace:
    case vee::LogSeverity::Debug:
    case vee::LogSeverity::Info:
        return vee::Color::Faint;
    case vee::LogSeverity::Warning:
        return vee::Color::Yellow;
    case vee::LogSeverity::Error:
    case vee::LogSeverity::Fatal:
        return vee::Color::Red;
    }
}

void vee::_log_impl(LogSeverity severity, std::string_view msg, const std::source_location& location) {
    auto time = ColoredExpr(std::format("[{:%H:%M:%S}]", std::chrono::system_clock::now()), Color::Faint);
    auto colored_severity = ColoredExpr(severity, get_severity_color(severity));

    // Make sure that we only add our own newline if the message doesn't already end with one.
    const char* auto_newline = msg.ends_with('\n') ? "" : "\n";
    auto full_msg = ColoredExpr(std::format(": {}{}", msg, auto_newline), get_severity_color(severity));

    switch (severity) {
    default:
        std::print(std::clog, "{} {} {}", time, colored_severity, full_msg);
        break;
    case LogSeverity::Error:
    case LogSeverity::Fatal:
        // Errors and Fatals get source location
        auto colored_loc = ColoredExpr(
            std::format("{}:{} ({})", location.file_name(), location.line(), LastNExpr(location.function_name(), 30)), Color::Cyan
        );
        std::print(std::clog, "{} {} {} {}", time, colored_severity, colored_loc, full_msg);
        break;
    }
}