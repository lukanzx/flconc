#pragma once

namespace flconc {

    enum {
        kRBP = 6,
        kRDI = 7,
        kRSI = 8,
        kRETAddr = 9,
        kRSP = 13,
    };

    struct coctx {
        void *regs[14];
    };

    extern "C" {
    inline void coctx_swap(coctx *old_ctx, coctx *new_ctx) {
        asm volatile (
                "leaq (%%rsp),%%rax\n"
                "movq %%rax, 104(%%rdi)\n"
                "movq %%rbx, 96(%%rdi)\n"
                "movq %%rcx, 88(%%rdi)\n"
                "movq %%rdx, 80(%%rdi)\n"
                "movq 0(%%rax), %%rax\n"
                "movq %%rax, 72(%%rdi)\n"
                "movq %%rsi, 64(%%rdi)\n"
                "movq %%rdi, 56(%%rdi)\n"
                "movq %%rbp, 48(%%rdi)\n"
                "movq %%r8, 40(%%rdi)\n"
                "movq %%r9, 32(%%rdi)\n"
                "movq %%r12, 24(%%rdi)\n"
                "movq %%r13, 16(%%rdi)\n"
                "movq %%r14, 8(%%rdi)\n"
                "movq %%r15, (%%rdi)\n"
                "xorq %%rax, %%rax\n"
                "movq 48(%%rsi), %%rbp\n"
                "movq 104(%%rsi), %%rsp\n"
                "movq (%%rsi), %%r15\n"
                "movq 8(%%rsi), %%r14\n"
                "movq 16(%%rsi), %%r13\n"
                "movq 24(%%rsi), %%r12\n"
                "movq 32(%%rsi), %%r9\n"
                "movq 40(%%rsi), %%r8\n"
                "movq 56(%%rsi), %%rdi\n"
                "movq 80(%%rsi), %%rdx\n"
                "movq 88(%%rsi), %%rcx\n"
                "movq 96(%%rsi), %%rbx\n"
                "leaq 8(%%rsp), %%rsp\n"
                "pushq 72(%%rsi)\n"
                "movq 64(%%rsi), %%rsi\n"
                "ret\n"
                :
                : "D" (old_ctx), "S" (new_ctx)
                : "memory"
                );
    }
    };

}
