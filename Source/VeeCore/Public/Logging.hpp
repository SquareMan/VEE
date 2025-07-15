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


#pragma once

#include <Debugging.hpp>

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
struct format_string_with_location {
    std::format_string<Args...> str;
    std::source_location location;

    template <typename T>
        requires std::constructible_from<std::format_string<Args...>, const T&>
    consteval format_string_with_location(const T& str, const std::source_location loc = std::source_location::current())
        : str(str)
        , location(loc) {}

    void log(log::Severity severity, Args&&... args) const {
        _log_impl(severity, std::format(str, std::forward<Args>(args)...), location);
    }
};
// We need this specialization for const char* because a non-literal const char* cannot be used to
// construct a format_string.
template <>
struct format_string_with_location<> {
    const char* str;
    std::source_location location;

    format_string_with_location(const char* str, std::source_location loc = std::source_location::current())
        : str(str)
        , location(loc) {}

    void log(log::Severity severity) {
        _log_impl(severity, str, location);
    }
};

template <typename... Args>
void log_trace(format_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    fmt.log(log::Severity::Trace, std::forward<Args>(args)...);
}
template <typename... Args>
void log_debug(format_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    fmt.log(log::Severity::Debug, std::forward<Args>(args)...);
}
template <typename... Args>
void log_info(format_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    fmt.log(log::Severity::Info, std::forward<Args>(args)...);
}
template <typename... Args>
void log_warning(format_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    fmt.log(log::Severity::Warning, std::forward<Args>(args)...);
}
template <typename... Args>
void log_error(format_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    fmt.log(log::Severity::Error, std::forward<Args>(args)...);
}
template <typename... Args>
[[noreturn]] void log_fatal(format_string_with_location<std::type_identity_t<Args>...> fmt, Args&&... args) {
    fmt.log(log::Severity::Fatal, std::forward<Args>(args)...);
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
