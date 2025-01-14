//
// Created by Square on 12/22/2024.
//
#include "Logging.hpp"

#include <array>
#include <boost/core/null_deleter.hpp>
#include <boost/log/attributes/clock.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup.hpp>
#include <boost/phoenix.hpp>
#include <iostream>
#include <magic_enum/magic_enum.hpp>

namespace attrs = boost::log::attributes;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
namespace sources = boost::log::sources;

std::ostream& operator<<(std::ostream& strm, Severity severity) {
    return strm << magic_enum::enum_name(severity);
}

enum class Color { Blue, Cyan, Faint, Green, Magenta, Red, Yellow, SIZE };
auto colored_expression(auto fmt, Color color) {
    return expr::wrap_formatter([fmt, color](const boost::log::record_view& record_view, boost::log::formatting_ostream& stream) {
        static constexpr auto color_codes = std::array{
            "\033[34m",
            "\033[36m",
            "\033[2;90m",
            "\033[32m",
            "\033[35m",
            "\033[31m",
            "\033[33m",
        };
        static_assert(color_codes.size() == static_cast<uint32_t>(Color::SIZE));

        stream << color_codes[static_cast<uint32_t>(color)];
        fmt(record_view, stream);
        stream << "\033[0;10m";
    });
}

std::string_view last_n_chars(const boost::log::value_ref<std::string>& value_ref, size_t n) {
    if (!value_ref) {
        return "";
    }

    auto& val = value_ref.get();
    if (val.size() <= n) {
        return val;
    }
    return {val.data() + val.size() - n, n};
}

BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Severity)

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(gLogger, sources::severity_logger_mt<Severity>) {
    // Library Core Initialization
    boost::log::add_common_attributes();
    boost::log::register_simple_formatter_factory<Severity, char>("Severity");
    auto core = boost::log::core::get();

    // Add file sink
    boost::log::add_file_log(keywords::file_name = ".log", keywords::format = "%TimeStamp% %Severity% %File%:%Line% (%Function%) : %Message%");

    // Setup and add console sink
    auto console_sink = boost::make_shared<sinks::synchronous_sink<sinks::text_ostream_backend>>();
    console_sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    // clang-format off
    console_sink->set_formatter(
        expr::stream
        << colored_expression(expr::stream << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f"), Color::Faint) << " "
        << expr::if_(is_in_range(severity, Severity::Trace, Severity::Warning))[
            expr::stream << colored_expression(expr::stream << expr::attr<Severity>("Severity"), Color::Faint)
        ].else_[
            expr::stream << expr::if_(is_in_range(severity, Severity::Warning, Severity::Error))[
                expr::stream << colored_expression(expr::stream << expr::attr<Severity>("Severity"), Color::Yellow)
            ].else_[
                expr::stream << colored_expression(expr::stream << expr::attr<Severity>("Severity"), Color::Red)
            ]
        ]
        << " " << colored_expression(
            expr::stream
            << boost::phoenix::bind(&last_n_chars, expr::attr<std::string>("File"), 30)
            << ":" << expr::attr<uint32_t>("Line")
            << " (" << expr::attr<std::string>("Function") << ")",
            Color::Cyan)
        << colored_expression(
            expr::stream
            << " : ", Color::Faint)
        << expr::if_(is_in_range(severity, Severity::Trace, Severity::Error))[
                expr::stream << expr::smessage
            ].else_[
                expr::stream << colored_expression(expr::stream << expr::smessage, Color::Red)
            ]
        << expr::auto_newline
    );
    // clang-format on
    core->add_sink(console_sink);

    // Global logger initialization
    sources::severity_logger_mt<Severity> lg;
    lg.add_attribute("File", attrs::mutable_constant<std::string>(""));
    lg.add_attribute("Line", attrs::mutable_constant<uint32_t>(0));
    lg.add_attribute("Function", attrs::mutable_constant<std::string>(""));
    return lg;
}

void vee::_log_msg(Severity severity, std::string msg, const std::source_location& location) {
    auto& logger = gLogger::get();
    auto attrs = logger.get_attributes();
    boost::log::attribute_cast<attrs::mutable_constant<std::string>>(attrs["File"]).set(location.file_name());
    boost::log::attribute_cast<attrs::mutable_constant<uint32_t>>(attrs["Line"]).set(location.line());
    boost::log::attribute_cast<attrs::mutable_constant<std::string>>(attrs["Function"]).set(location.function_name());

    BOOST_LOG_SEV(logger, severity) << msg;
}