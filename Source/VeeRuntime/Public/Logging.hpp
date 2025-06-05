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

template <typename... Args>
struct log_trace : _log_fmt<log::Severity::Trace, Args...> {
    explicit log_trace(const std::format_string<Args...>& fmt, Args&&... param, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Trace, Args...>(fmt, std::forward<Args>(param)..., loc) {}
    log_trace(const char* msg, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Trace, Args...>(msg, loc) {}
};
template <typename... Args>
log_trace(std::format_string<Args...> fmt, Args&&... args) -> log_trace<Args...>;

template <typename... Args>
struct log_info : _log_fmt<log::Severity::Info, Args...> {
    explicit log_info(const std::format_string<Args...>& fmt, Args&&... param, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Info, Args...>(fmt, std::forward<Args>(param)..., loc) {}
    log_info(const char* msg, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Info, Args...>(msg, loc) {}
};
template <typename... Args>
log_info(std::format_string<Args...> fmt, Args&&... args) -> log_info<Args...>;

template <typename... Args>
struct log_debug : _log_fmt<log::Severity::Debug, Args...> {
    explicit log_debug(const std::format_string<Args...>& fmt, Args&&... param, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Debug, Args...>(fmt, std::forward<Args>(param)..., loc) {}
    log_debug(const char* msg, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Debug, Args...>(msg, loc) {}
};
template <typename... Args>
log_debug(std::format_string<Args...> fmt, Args&&... args) -> log_debug<Args...>;

template <typename... Args>
struct log_warning : _log_fmt<log::Severity::Warning, Args...> {
    explicit log_warning(const std::format_string<Args...>& fmt, Args&&... param, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Warning, Args...>(fmt, std::forward<Args>(param)..., loc) {}
    log_warning(const char* msg, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Warning, Args...>(msg, loc) {}
};
template <typename... Args>
log_warning(std::format_string<Args...> fmt, Args&&... args) -> log_warning<Args...>;

template <typename... Args>
struct log_error : _log_fmt<log::Severity::Error, Args...> {
    explicit log_error(const std::format_string<Args...>& fmt, Args&&... param, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Error, Args...>(fmt, std::forward<Args>(param)..., loc) {}
    log_error(const char* msg, const std::source_location& loc = std::source_location::current())
        : _log_fmt<log::Severity::Error, Args...>(msg, loc) {}
};
template <typename... Args>
log_error(std::format_string<Args...> fmt, Args&&... args) -> log_error<Args...>;


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
