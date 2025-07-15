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

#define VFIBER_RIP_OFFSET 0x00
#define VFIBER_RSP_OFFSET 0x08

#define VFIBER_RBP_OFFSET 0x10
#define VFIBER_RBX_OFFSET 0x18
#define VFIBER_RDI_OFFSET 0x20
#define VFIBER_RSI_OFFSET 0x28
#define VFIBER_R12_OFFSET 0x30
#define VFIBER_R13_OFFSET 0x38
#define VFIBER_R14_OFFSET 0x40
#define VFIBER_R15_OFFSET 0x48

#define VFIBER_XMM6_OFFSET 0x50
#define VFIBER_XMM7_OFFSET 0x58
#define VFIBER_XMM8_OFFSET 0x60
#define VFIBER_XMM9_OFFSET 0x68
#define VFIBER_XMM10_OFFSET 0x70
#define VFIBER_XMM11_OFFSET 0x78
#define VFIBER_XMM12_OFFSET 0x80
#define VFIBER_XMM13_OFFSET 0x88
#define VFIBER_XMM14_OFFSET 0x90
#define VFIBER_XMM15_OFFSET 0x98


#ifndef ASSEMBLY
#include <cstddef>
#include <cstdint>

// TODO: Implement other platforms
namespace vee {
struct FiberContext {
    std::uintptr_t rip;
    std::uintptr_t rsp;

    std::uintptr_t rbp;
    std::uintptr_t rbx;
    std::uintptr_t rdi;
    std::uintptr_t rsi;
    std::uintptr_t r12;
    std::uintptr_t r13;
    std::uintptr_t r14;
    std::uintptr_t r15;

    std::uintptr_t xmm6;
    std::uintptr_t xmm7;
    std::uintptr_t xmm8;
    std::uintptr_t xmm9;
    std::uintptr_t xmm10;
    std::uintptr_t xmm11;
    std::uintptr_t xmm12;
    std::uintptr_t xmm13;
    std::uintptr_t xmm14;
    std::uintptr_t xmm15;
};
static_assert(offsetof(FiberContext, rip) == VFIBER_RIP_OFFSET);
static_assert(offsetof(FiberContext, rsp) == VFIBER_RSP_OFFSET);
static_assert(offsetof(FiberContext, rbp) == VFIBER_RBP_OFFSET);
static_assert(offsetof(FiberContext, rbx) == VFIBER_RBX_OFFSET);
static_assert(offsetof(FiberContext, rdi) == VFIBER_RDI_OFFSET);
static_assert(offsetof(FiberContext, rsi) == VFIBER_RSI_OFFSET);
static_assert(offsetof(FiberContext, r12) == VFIBER_R12_OFFSET);
static_assert(offsetof(FiberContext, r13) == VFIBER_R13_OFFSET);
static_assert(offsetof(FiberContext, r14) == VFIBER_R14_OFFSET);
static_assert(offsetof(FiberContext, r15) == VFIBER_R15_OFFSET);
static_assert(offsetof(FiberContext, xmm6) == VFIBER_XMM6_OFFSET);
static_assert(offsetof(FiberContext, xmm7) == VFIBER_XMM7_OFFSET);
static_assert(offsetof(FiberContext, xmm8) == VFIBER_XMM8_OFFSET);
static_assert(offsetof(FiberContext, xmm9) == VFIBER_XMM9_OFFSET);
static_assert(offsetof(FiberContext, xmm10) == VFIBER_XMM10_OFFSET);
static_assert(offsetof(FiberContext, xmm11) == VFIBER_XMM11_OFFSET);
static_assert(offsetof(FiberContext, xmm12) == VFIBER_XMM12_OFFSET);
static_assert(offsetof(FiberContext, xmm13) == VFIBER_XMM13_OFFSET);
static_assert(offsetof(FiberContext, xmm14) == VFIBER_XMM14_OFFSET);
static_assert(offsetof(FiberContext, xmm15) == VFIBER_XMM15_OFFSET);
} // namespace vee


// Avoid polluting preprocessor defines in C++, we only need these defines for the assembler.
#undef VFIBER_RIP_OFFSET
#undef VFIBER_RSP_OFFSET
#undef VFIBER_RBP_OFFSET
#undef VFIBER_RBX_OFFSET
#undef VFIBER_RDI_OFFSET
#undef VFIBER_RSI_OFFSET
#undef VFIBER_R12_OFFSET
#undef VFIBER_R13_OFFSET
#undef VFIBER_R14_OFFSET
#undef VFIBER_R15_OFFSET
#undef VFIBER_XMM6_OFFSET
#undef VFIBER_XMM7_OFFSET
#undef VFIBER_XMM8_OFFSET
#undef VFIBER_XMM9_OFFSET
#undef VFIBER_XMM10_OFFSET
#undef VFIBER_XMM11_OFFSET
#undef VFIBER_XMM12_OFFSET
#undef VFIBER_XMM13_OFFSET
#undef VFIBER_XMM14_OFFSET
#undef VFIBER_XMM15_OFFSET

#endif
