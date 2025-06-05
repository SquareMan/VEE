//
// Created by Square on 12/20/2024.
//
#pragma once

#include "VeeCore.hpp"

#include <format>
#include <source_location>

namespace vee {

namespace log {
enum class Severity {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};
}

void _log_msg(log::Severity severity, std::string_view msg, const std::source_location& location);

template <const log::Severity severity, typename... Args>
struct _log_fmt {
    explicit _log_fmt(std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
        const std::string msg = std::format(fmt, std::forward<Args>(args)...);
        _log_msg(severity, msg, loc);
    }

    explicit _log_fmt(const char* msg, const std::source_location& loc = std::source_location::current()) {
        _log_msg(severity, msg, loc);
    }
};

template <const log::Severity severity, typename... Args>
_log_fmt(std::format_string<Args...> fmt, Args&&... args) -> _log_fmt<severity, Args...>;

template <typename... Args>
using log_trace = _log_fmt<log::Severity::Trace, Args...>;

template <typename... Args>
using log_debug = _log_fmt<log::Severity::Debug, Args...>;

template <typename... Args>
using log_info = _log_fmt<log::Severity::Info, Args...>;

template <typename... Args>
using log_warning = _log_fmt<log::Severity::Warning, Args...>;

template <typename... Args>
using log_error = _log_fmt<log::Severity::Error, Args...>;

template <typename... Args>
struct log_fatal {
    [[noreturn]] explicit log_fatal(std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
        _log_fmt<log::Severity::Fatal, Args...>(fmt, std::forward<Args>(args)..., loc);
        VEE_DEBUGBREAK();
        std::abort();
    }

    [[noreturn]] explicit log_fatal(const char* msg, const std::source_location& loc = std::source_location::current()) {
        _log_fmt<log::Severity::Fatal>(msg, loc);
        VEE_DEBUGBREAK();
        std::abort();
    }
};
template <typename... Args>
log_fatal(std::format_string<Args...> fmt, Args&&... args) -> log_fatal<Args...>;

template <typename... Args>
struct log_dynamic {
    log_dynamic(log::Severity severity, std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
        switch (severity) {
        case log::Severity::Trace:
            log_trace<Args...>(fmt, std::forward<Args>(args)..., loc);
            break;
        case log::Severity::Debug:
            log_debug<Args...>(fmt, std::forward<Args>(args)..., loc);
            break;
        case log::Severity::Info:
            log_info<Args...>(fmt, std::forward<Args>(args)..., loc);
            break;
        case log::Severity::Warning:
            log_warning<Args...>(fmt, std::forward<Args>(args)..., loc);
            break;
        case log::Severity::Error:
            log_error<Args...>(fmt, std::forward<Args>(args)..., loc);
            break;
        case log::Severity::Fatal:
            log_fatal<Args...>(fmt, std::forward<Args>(args)..., loc);
            break;
        }
    }
};
template <typename... Args>
log_dynamic(log::Severity severity, std::format_string<Args...> fmt, Args&&... args) -> log_dynamic<Args...>;

} // namespace vee
