//
// Created by Square on 1/22/2025.
//

#pragma once

#include "Assert.hpp"
#include "Logging.hpp"

#include <array>
#include <format>
#include <magic_enum/magic_enum.hpp>

template <>
struct std::formatter<vee::log::Severity> {
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }

    static auto format(vee::log::Severity severity, format_context& ctx) {
        return std::format_to(ctx.out(), "{}", magic_enum::enum_name(severity));
    }
};

struct LastN {
    std::string_view view;
    uint32_t n;
};

template <>
struct std::formatter<LastN> {
    static constexpr auto parse(format_parse_context& ctx) {
        return ctx.begin();
    }
    static auto format(LastN str, format_context& ctx) {
        if (str.view.size() > str.n) {
            return std::format_to(ctx.out(), "...{}", string_view(str.view.data() + str.view.size() - str.n));
        }
        return std::formatter<string_view>().format(str.view, ctx);
    }
};

namespace vee::log {
enum class Color { Blue, Cyan, Faint, Green, Magenta, Red, Yellow, SIZE };
template <typename T>
struct ColoredExpr {
    T t;
    Color color;
};
} // namespace vee::log

template <typename T>
struct std::formatter<vee::log::ColoredExpr<T>> {
    static constexpr auto parse(format_parse_context& ctx) {
        return std::formatter<T>().parse(ctx);
    }

    static auto format(vee::log::ColoredExpr<T>& expr, format_context& ctx) {
        VASSERT(expr.color != vee::log::Color::SIZE, "SIZE is not a valid color.");
        static constexpr auto color_codes = std::array{
            "\033[34m",
            "\033[36m",
            "\033[2;90m",
            "\033[32m",
            "\033[35m",
            "\033[31m",
            "\033[33m",
        };
        static_assert(color_codes.size() == static_cast<uint32_t>(vee::log::Color::SIZE));

        const char* code = color_codes[static_cast<uint32_t>(expr.color)];
        return std::format_to(ctx.out(), "{}{}\033[0;10m", code, expr.t);
    }
};
// } // namespace vee::log
