//
// Created by Square on 1/22/2025.
//

#include "Assert.hpp"

#include "LogUtil.hpp"

#include <boost/core/null_deleter.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/phoenix.hpp>
#include <iostream>

BOOST_LOG_INLINE_GLOBAL_LOGGER_INIT(g_assert_logger, boost::log::sources::severity_logger_mt<Severity>) {
    using namespace boost::log;
    using namespace vee::log;
    namespace expr = expressions;

    add_common_attributes();

    // Add file sink
    add_file_log(keywords::file_name = ".log", keywords::format = "%TimeStamp% %File%:%Line% (%Function%) : Assertion Failed: %Expression%\n\t%Message%", keywords::filter = severity >= Severity::Assert);

    // Setup and add console sink
    auto console_sink = boost::make_shared<sinks::synchronous_sink<sinks::text_ostream_backend>>();
    console_sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>(&std::clog, boost::null_deleter()));
    // clang-format off
    console_sink->set_formatter(
        expr::stream
        << colored_expression(expr::stream << expr::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f"), Color::Faint) << " "
        << colored_expression(expr::stream << "ASSERT ", Color::Red)
        << colored_expression(
            expr::stream
            << boost::phoenix::bind(&last_n_chars, expr::attr<std::string>("File"), 30)
            << ":" << expr::attr<uint32_t>("Line")
            << " (" << expr::attr<std::string>("Function") << ")",
            Color::Cyan)
        << colored_expression(
            expr::stream
            << " : ", Color::Faint)
        << colored_expression(expr::stream << expr::attr<const char*>("Expression"), Color::Red)
        << "\n\t"
        << expr::smessage
        << expr::auto_newline
    );
    console_sink->set_filter(severity >= Severity::Assert);
    // clang-format on
    core::get()->add_sink(console_sink);

    // Assert logger initialization
    sources::severity_logger_mt<Severity> lg;
    lg.add_attribute("File", attributes::mutable_constant<std::string>(""));
    lg.add_attribute("Line", attributes::mutable_constant<uint32_t>(0));
    lg.add_attribute("Function", attributes::mutable_constant<std::string>(""));
    lg.add_attribute("Expression", attributes::mutable_constant<const char*>(""));
    return lg;
}

void vee::assert::detail::_log_assert(const char* expr_str, const std::source_location& loc, std::string_view msg) {
    using namespace boost::log;
    auto& logger = g_assert_logger::get();
    auto attrs = logger.get_attributes();
    boost::log::attribute_cast<attributes::mutable_constant<std::string>>(attrs["File"]).set(loc.file_name());
    boost::log::attribute_cast<attributes::mutable_constant<uint32_t>>(attrs["Line"]).set(loc.line());
    boost::log::attribute_cast<attributes::mutable_constant<std::string>>(attrs["Function"]).set(loc.function_name());
    boost::log::attribute_cast<attributes::mutable_constant<const char*>>(attrs["Expression"]).set(expr_str);

    BOOST_LOG_SEV(logger, Severity::Assert) << msg;
}