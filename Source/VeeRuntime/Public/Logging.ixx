//
// Created by Square on 12/20/2024.
//
module;

#include "VeeCore.hpp"

#include <source_location>
#include <format>

export module Logging;

export enum class Severity {
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

std::ostream& operator<<(std::ostream& strm, Severity severity);

namespace vee {

void _log_msg(Severity severity, std::string msg, const std::source_location& location);

export template <const Severity severity, typename... Args>
struct _log_fmt {
    explicit _log_fmt(std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
        const std::string msg = std::format(fmt, std::forward<Args>(args)...);
        _log_msg(severity, msg, loc);
    }
};
export template <const Severity severity, typename... Args>
_log_fmt(std::format_string<Args...> fmt, Args&&... args) -> _log_fmt<severity, Args...>;

export template <typename... Args>
using log_trace = _log_fmt<Severity::Trace, Args...>;

export template <typename... Args>
using log_debug = _log_fmt<Severity::Debug, Args...>;

export template <typename... Args>
using log_info = _log_fmt<Severity::Info, Args...>;

export template <typename... Args>
using log_warning = _log_fmt<Severity::Warning, Args...>;

export template <typename... Args>
using log_error = _log_fmt<Severity::Error, Args...>;

export template <typename... Args>
struct log_fatal {
    [[noreturn]] explicit log_fatal(std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
        _log_fmt<Severity::Fatal, Args...>(fmt, std::forward<Args>(args)..., loc);
        VEE_DEBUGBREAK();
        std::exit(-1);
    }
};
export template <typename... Args>
log_fatal(std::format_string<Args...> fmt, Args&&... args) -> log_fatal<Args...>;

export template <typename... Args>
struct log_dynamic {
    log_dynamic(Severity severity, std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()
    ) {
        switch (severity) {
        case Severity::Trace:
            log_trace(fmt, std::forward<Args>(args)...);
            break;
        case Severity::Debug:
            log_debug(fmt, std::forward<Args>(args)...);
            break;
        case Severity::Info:
            log_info(fmt, std::forward<Args>(args)...);
            break;
        case Severity::Warning:
            log_warning(fmt, std::forward<Args>(args)...);
            break;
        case Severity::Error:
            log_error(fmt, std::forward<Args>(args)...);
            break;
        case Severity::Fatal:
            log_fatal(fmt, std::forward<Args>(args)...);
            break;
        }
    }
};
export template <typename... Args>
log_dynamic(Severity severity, std::format_string<Args...> fmt, Args&&... args) -> log_dynamic<Args...>;

} // namespace vee
