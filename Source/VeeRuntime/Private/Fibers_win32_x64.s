#include "FiberContext_win32_x64.h"

.text

.align 16
.global "?fiber_context_switch@vee@@YAXPEAUFiberContext@1@PEBU21@@Z"
"?fiber_context_switch@vee@@YAXPEAUFiberContext@1@PEBU21@@Z":
    #Use the return address of this function for resuming the current fiber later.
    mov (%rsp), %rax
    mov %rax, VFIBER_RIP_OFFSET(%rcx)
    
    #Save the stack pointer of the current fiber
    lea 0x8(%rsp), %rax
    mov %rax, VFIBER_RSP_OFFSET(%rcx)

    #Store non-voltatile registers
    mov %rbp, VFIBER_RBP_OFFSET(%rcx)
    mov %rbx, VFIBER_RBX_OFFSET(%rcx)
    mov %rdi, VFIBER_RDI_OFFSET(%rcx)
    mov %rsi, VFIBER_RSI_OFFSET(%rcx)
    mov %r12, VFIBER_R12_OFFSET(%rcx)
    mov %r13, VFIBER_R13_OFFSET(%rcx)
    mov %r14, VFIBER_R14_OFFSET(%rcx)
    mov %r15, VFIBER_R15_OFFSET(%rcx)
    mov %xmm6, VFIBER_XMM6_OFFSET(%rcx)
    mov %xmm7, VFIBER_XMM7_OFFSET(%rcx)
    mov %xmm8, VFIBER_XMM8_OFFSET(%rcx)
    mov %xmm9, VFIBER_XMM9_OFFSET(%rcx)
    mov %xmm10, VFIBER_XMM10_OFFSET(%rcx)
    mov %xmm11, VFIBER_XMM11_OFFSET(%rcx)
    mov %xmm12, VFIBER_XMM12_OFFSET(%rcx)
    mov %xmm13, VFIBER_XMM13_OFFSET(%rcx)
    mov %xmm14, VFIBER_XMM14_OFFSET(%rcx)
    mov %xmm15, VFIBER_XMM15_OFFSET(%rcx)
    
    # Load destination non-volatile registers
    mov VFIBER_RBP_OFFSET(%rdx), %rbp
    mov VFIBER_RBX_OFFSET(%rdx), %rbx
    mov VFIBER_RDI_OFFSET(%rdx), %rdi
    mov VFIBER_RSI_OFFSET(%rdx), %rsi
    mov VFIBER_R12_OFFSET(%rdx), %r12
    mov VFIBER_R13_OFFSET(%rdx), %r13
    mov VFIBER_R14_OFFSET(%rdx), %r14
    mov VFIBER_R15_OFFSET(%rdx), %r15
    mov VFIBER_XMM6_OFFSET(%rdx), %xmm6
    mov VFIBER_XMM7_OFFSET(%rdx), %xmm7
    mov VFIBER_XMM8_OFFSET(%rdx), %xmm8
    mov VFIBER_XMM9_OFFSET(%rdx), %xmm9
    mov VFIBER_XMM10_OFFSET(%rdx), %xmm10
    mov VFIBER_XMM11_OFFSET(%rdx), %xmm11
    mov VFIBER_XMM12_OFFSET(%rdx), %xmm12
    mov VFIBER_XMM13_OFFSET(%rdx), %xmm13
    mov VFIBER_XMM14_OFFSET(%rdx), %xmm14
    mov VFIBER_XMM15_OFFSET(%rdx), %xmm15

    # Restore stack pointer
    mov VFIBER_RSP_OFFSET(%rdx), %rsp

    # Load fiber argument
    mov VFIBER_ARG_OFFSET(%rdx), %rcx
    
    # Restore instruction pointer
    mov VFIBER_RIP_OFFSET(%rdx), %rdx
    jmp *%rdx
    
