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
    { T::PreHandle(c, loc, fmt, std::forward<FmtArgs>(args)...) };
    { T::Handle() };
};

#ifndef VEE_ASSERT_FILTER_LEVEL
#define VEE_ASSERT_FILTER_LEVEL Release
#endif

namespace detail {
void _log_assert(const char* expr_str, const std::source_location& loc, std::string_view msg);
};

struct DefaultHandler {
    constexpr static auto FilterLevel = Level::VEE_ASSERT_FILTER_LEVEL;
    constexpr static auto DefaultLevel = Level::Debug;
    template <typename... FmtArgs>
    constexpr static void PreHandle(const char* c, const std::source_location& loc, std::format_string<FmtArgs...> fmt = "", FmtArgs&&... args) {
        detail::_log_assert(c, loc, std::format(fmt, std::forward<FmtArgs>(args)...));
    }
    constexpr static void Handle() {
        std::abort();
    }
};

namespace detail {
template <IsHandler Handler, Level level, std::predicate<> Expr>
void assert_impl(Expr expr) {
    static_assert(level > Level::Off, "An assert should not be off by default.");
    if constexpr (Handler::FilterLevel >= level) {
        if (!expr()) {
            Handler::Handle();
        }
    }
}
} // namespace detail
}; // namespace vee::assert


#define VASSERT(Expr, ...) VASSERT_WITH_HANDLER(::vee::assert::DefaultHandler, Expr, __VA_ARGS__)

#define VASSERT_WITH_HANDLER(Handler, Expr, ...)                                                   \
    ::vee::assert::detail::assert_impl<Handler, Handler::DefaultLevel>([&] {                       \
        if (!(Expr)) {                                                                             \
            Handler::PreHandle(#Expr, std::source_location::current() __VA_OPT__(, ) __VA_ARGS__); \
            VEE_DEBUGBREAK();                                                                      \
            return false;                                                                          \
        }                                                                                          \
        return true;                                                                               \
    })
