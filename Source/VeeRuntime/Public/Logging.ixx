//
// Created by Square on 12/20/2024.
//
module;

#include "VeeCore.hpp"

#include <boost/log/common.hpp>
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

namespace sources = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

namespace vee {

void _log(Severity severity, std::string msg, const std::source_location& location);

#define IMPL_LOG(name, severity)                                                                                                            \
    export template <typename... Args>                                                                                                      \
    struct name {                                                                                                                           \
        explicit name(std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) { \
            const std::string msg = std::format(fmt, std::forward<Args>(args)...);                                                          \
            _log(severity, msg, loc);                                                                                                       \
        }                                                                                                                                   \
    };                                                                                                                                      \
    export template <typename... Args>                                                                                                      \
    name(std::format_string<Args...> fmt, Args&&... args) -> name<Args...>;

IMPL_LOG(log_trace, Severity::Trace)
IMPL_LOG(log_debug, Severity::Debug)
IMPL_LOG(log_info, Severity::Info)
IMPL_LOG(log_warning, Severity::Warning)
IMPL_LOG(log_error, Severity::Error)
export template <typename... Args>
struct log_fatal {
    [[noreturn]] explicit log_fatal(std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()) {
        const std::string msg = std::format(fmt, std::forward<Args>(args)...);
        _log(Severity::Fatal, msg, loc);
        VEE_DEBUGBREAK();
        std::exit(-1);
    }
};
export template <typename... Args>
log_fatal(std::format_string<Args...> fmt, Args&&... args) -> log_fatal<Args...>;

export template <typename... Args>
struct log {
    log(Severity severity, std::format_string<Args...> fmt, Args&&... args, const std::source_location& loc = std::source_location::current()
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
log(Severity severity, std::format_string<Args...> fmt, Args&&... args) -> log<Args...>;

} // namespace vee
