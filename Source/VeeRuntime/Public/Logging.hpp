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
void _log_impl(log::Severity severity, std::string_view msg, const std::source_location& location);

// (Ab)use implicit constructor to allow us to provide the default value for the source_location,
// while still accepting variadic arguments in our actual logging functions. When a log function is
// called, the format string will be coerced to this type.
template <typename... Args>
struct fmt_string_with_location {
    std::format_string<Args...> str;
    std::source_location location;

    template <typename T>
        requires std::constructible_from<std::format_string<Args...>, const T&>
    consteval fmt_string_with_location(const T& str, const std::source_location loc = std::source_location::current())
        : str(str)
        , location(loc) {}
};

template <typename... Args>
void log_trace(fmt_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    _log_impl(log::Severity::Trace, std::format(fmt.str, std::forward<Args>(args)...), fmt.location);
}
// We need a specific const char* version of each log function because a non-literal char* can't be
// used to construct a std::format_string
// TODO: It may be possible to handle this by creating a second version of fmt_string_with_location
// that takes and stores just a const char*, and then select the appropriate one with
// concepts/SFINAE
inline void log_trace(const char* str, const std::source_location& loc = std::source_location::current()) {
    _log_impl(log::Severity::Trace, str, loc);
}


template <typename... Args>
void log_debug(fmt_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    _log_impl(log::Severity::Debug, std::format(fmt.str, std::forward<Args>(args)...), fmt.location);
}
inline void log_debug(const char* str, const std::source_location& loc = std::source_location::current()) {
    _log_impl(log::Severity::Debug, str, loc);
}

template <typename... Args>
void log_info(fmt_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    _log_impl(log::Severity::Info, std::format(fmt.str, std::forward<Args>(args)...), fmt.location);
}
inline void log_info(const char* str, const std::source_location& loc = std::source_location::current()) {
    _log_impl(log::Severity::Info, str, loc);
}

template <typename... Args>
void log_warning(fmt_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    _log_impl(log::Severity::Warning, std::format(fmt.str, std::forward<Args>(args)...), fmt.location);
}
inline void log_warning(const char* str, const std::source_location& loc = std::source_location::current()) {
    _log_impl(log::Severity::Warning, str, loc);
}


template <typename... Args>
void log_error(fmt_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    _log_impl(log::Severity::Error, std::format(fmt.str, std::forward<Args>(args)...), fmt.location);
}
inline void log_error(const char* str, const std::source_location& loc = std::source_location::current()) {
    _log_impl(log::Severity::Error, str, loc);
}

template <typename... Args>
[[noreturn]] void log_fatal(fmt_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    _log_impl(log::Severity::Fatal, std::format(fmt.str, std::forward<Args>(args)...), fmt.location);
    VEE_DEBUGBREAK();
    std::abort();
}
[[noreturn]] inline void log_fatal(const char* msg, const std::source_location& loc = std::source_location::current()) {
    _log_impl(log::Severity::Fatal, msg, loc);
    VEE_DEBUGBREAK();
    std::abort();
}


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
