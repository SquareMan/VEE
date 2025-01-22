//
// Created by Square on 1/22/2025.
//

#pragma once

#include <array>
#include <boost/log/expressions.hpp>

namespace vee::log {
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", Severity)

enum class Color { Blue, Cyan, Faint, Green, Magenta, Red, Yellow, SIZE };
auto colored_expression(auto fmt, Color color) {
    namespace expr = boost::log::expressions;
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

std::string_view last_n_chars(const boost::log::value_ref<std::string>& value_ref, size_t n);
} // namespace vee::log
