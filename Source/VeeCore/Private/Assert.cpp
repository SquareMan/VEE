//
// Created by Square on 1/22/2025.
//

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