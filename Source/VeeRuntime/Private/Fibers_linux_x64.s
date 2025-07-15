#include "FiberContext_linux_x64.h"

.text

.align 16
.global _ZN3vee20fiber_context_switchEPNS_12FiberContextEPKS0_
_ZN3vee20fiber_context_switchEPNS_12FiberContextEPKS0_:
    #Use the return address of this function for resuming the current fiber later.
    mov (%rsp), %rax
    mov %rax, VFIBER_RIP_OFFSET(%rdi)
    
    #Save the stack pointer of the current fiber
    lea 0x8(%rsp), %rax
    mov %rax, VFIBER_RSP_OFFSET(%rdi)

    #Store non-voltatile registers
    mov %rbp, VFIBER_RBP_OFFSET(%rdi)
    mov %rbx, VFIBER_RBX_OFFSET(%rdi)
    mov %r12, VFIBER_R12_OFFSET(%rdi)
    mov %r13, VFIBER_R13_OFFSET(%rdi)
    mov %r14, VFIBER_R14_OFFSET(%rdi)
    mov %r15, VFIBER_R15_OFFSET(%rdi)
    
    # Load destination non-volatile registers
    mov VFIBER_RBP_OFFSET(%rsi), %rbp
    mov VFIBER_RBX_OFFSET(%rsi), %rbx
    mov VFIBER_R12_OFFSET(%rsi), %r12
    mov VFIBER_R13_OFFSET(%rsi), %r13
    mov VFIBER_R14_OFFSET(%rsi), %r14
    mov VFIBER_R15_OFFSET(%rsi), %r15

    # Restore stack pointer
    mov VFIBER_RSP_OFFSET(%rsi), %rsp
    
    # Restore instruction pointer
    mov VFIBER_RIP_OFFSET(%rsi), %rcx
    jmp *%rcx
    
