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

#include <cstdint>

namespace vee {
enum class Key : int32_t {
    Unknown = -1,

    /* Printable keys */
    Space = 32,
    Apostrophe = 39, /* ' */
    Comma = 44,      /* , */
    Minus = 45,      /* - */
    Period = 46,     /* . */
    Slash = 47,      /* / */
    Num0 = 48,
    Num1 = 49,
    Num2 = 50,
    Num3 = 51,
    Num4 = 52,
    Num5 = 53,
    Num6 = 54,
    Num7 = 55,
    Num8 = 56,
    Num9 = 57,
    Semicolon = 59, /* ; */
    Equal = 61,     /* = */
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    Left_Bracket = 91,  /* [ */
    Backslash = 92,     /* \ */
    Right_Bracket = 93, /* ] */
    Grave_Accent = 96,  /* ` */
    World_1 = 161,      /* non-US #1 */
    World_2 = 162,      /* non-US #2 */

    /* Function keys */
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    Page_Up = 266,
    Page_Down = 267,
    Home = 268,
    End = 269,
    Caps_Lock = 280,
    Scroll_Lock = 281,
    Num_Lock = 282,
    Print_Screen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    Kp_0 = 320,
    Kp_1 = 321,
    Kp_2 = 322,
    Kp_3 = 323,
    Kp_4 = 324,
    Kp_5 = 325,
    Kp_6 = 326,
    Kp_7 = 327,
    Kp_8 = 328,
    Kp_9 = 329,
    Kp_Decimal = 330,
    Kp_Divide = 331,
    Kp_Multiply = 332,
    Kp_Subtract = 333,
    Kp_Add = 334,
    Kp_Enter = 335,
    Kp_Equal = 336,
    Left_Shift = 340,
    Left_Control = 341,
    Left_Alt = 342,
    Left_Super = 343,
    Right_Shift = 344,
    Right_Control = 345,
    Right_Alt = 346,
    Right_Super = 347,
    Menu = 348,
};
}