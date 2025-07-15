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

#include "Logging.hpp"

#include <cstdint>
#include <format>
#include <source_location>
#include <type_traits>


namespace vee::assert {

enum class Level : uint8_t {
    Off,
    Release,
    Debug,
    Slow,
};

constexpr static auto Release = std::integral_constant<Level, Level::Release>{};
constexpr static auto Debug = std::integral_constant<Level, Level::Debug>{};
constexpr static auto Slow = std::integral_constant<Level, Level::Slow>{};

template <typename T, typename... FmtArgs>
concept IsHandler = requires(const char* c, const std::source_location& loc, std::format_string<FmtArgs...> fmt, FmtArgs&&... args) {
    { T::FilterLevel };
    { T::DefaultLevel };
    { T::Handle(c, loc, fmt, std::forward<FmtArgs>(args)...) };
};

#ifndef VEE_ASSERT_FILTER_LEVEL
#if !defined(NDEBUG)
#define VEE_ASSERT_FILTER_LEVEL Slow
#else
#define VEE_ASSERT_FILTER_LEVEL Release
#endif
#endif

namespace detail {
void _log_assert(const char* expr_str, const std::source_location& loc, std::string_view msg);
};

struct DefaultHandler {
    constexpr static auto FilterLevel = Level::VEE_ASSERT_FILTER_LEVEL;
    constexpr static auto DefaultLevel = Level::Debug;
    template <typename... FmtArgs>
    constexpr static void Handle(const char* expr_str, const std::source_location& loc, std::format_string<FmtArgs...> fmt, FmtArgs&&... args) {
        detail::_log_assert(expr_str, loc, std::format<FmtArgs...>(fmt, std::forward<FmtArgs>(args)...));
    }
};

namespace detail {
template <std::predicate<> Expr, IsHandler Handler, Level level, typename... Args>
void assert_impl(
    Expr expr, const char* expr_str, const std::source_location& loc, Handler, std::integral_constant<Level, level>, const std::format_string<Args...>& fmt = "", Args&&... args
) {
    static_assert(level > Level::Off, "An assert should not be off by default.");
    if constexpr (Handler::FilterLevel >= level) {
        if (!expr()) {
            Handler::Handle(expr_str, loc, fmt, std::forward<Args>(args)...);
            std::abort();
        }
    }
}

template <std::predicate<> Expr, Level level, typename... Args>
void assert_impl(
    Expr expr, const char* expr_str, const std::source_location& loc, std::integral_constant<Level, level> l, const std::format_string<Args...>& fmt = "", Args&&... args
) {
    assert_impl<Expr, DefaultHandler, level, Args...>(expr, expr_str, loc, DefaultHandler{}, l, fmt, std::forward<Args>(args)...);
}

template <std::predicate<> Expr, IsHandler Handler, typename... Args>
void assert_impl(Expr expr, const char* expr_str, const std::source_location& loc, Handler handler, const std::format_string<Args...>& fmt = "", Args&&... args) {
    assert_impl<Expr, Handler, Handler::DefaultLevel, Args...>(
        expr, expr_str, loc, handler, std::integral_constant<Level, Handler::DefaultLevel>{}, fmt, std::forward<Args>(args)...
    );
}

template <std::predicate<> Expr, typename... Args>
void assert_impl(Expr expr, const char* expression_str, const std::source_location& loc, const std::format_string<Args...>& fmt = "", Args&&... args) {
    assert_impl<Expr, DefaultHandler, Level::Debug, Args...>(
        expr, expression_str, loc, DefaultHandler{}, std::integral_constant<Level, DefaultHandler::DefaultLevel>{}, fmt, std::forward<Args>(args)...
    );
}
} // namespace detail
}; // namespace vee::assert


#define VASSERT(Expr, ...)                                                                         \
    ::vee::assert::detail::assert_impl(                                                            \
        [&] {                                                                                      \
            if (!(Expr)) {                                                                         \
                VEE_DEBUGBREAK();                                                                  \
                return false;                                                                      \
            }                                                                                      \
            return true;                                                                           \
        },                                                                                         \
        #Expr,                                                                                     \
        std::source_location::current(),                                                           \
        ##__VA_ARGS__                                                                              \
    )
