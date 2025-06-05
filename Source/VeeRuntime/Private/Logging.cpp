//
// Created by Square on 12/22/2024.
//
#include "Logging.hpp"
#include "LogUtil.hpp"

#include <chrono>
#include <iostream>



void vee::_log_msg(log::Severity severity, std::string_view msg, const std::source_location& location) {
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