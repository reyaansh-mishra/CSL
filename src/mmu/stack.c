/* src/mmu/stack.c */

#include <utils.h>
#include <csl.h>
#include <stack.h>
#include <arm64.h>
#include <terminal.h>

static uint8_t csl_stack[CSL_STACK_SIZE] __attribute__((aligned(16)));

void setup_stack(uintptr_t next_func) {
    __asm__ volatile(
        "msr SPSel, #1\n"     // Switch to SP_EL2, EL2h
        "isb\n"
        "mov x1, %0\n"       // stash next_func in x1
        "mov sp, %1\n"       // switch stack pointer directly, no function call
        "br x1\n"            // jump using the stashed target
        :
        : "r"(next_func), "r"((uint64_t)csl_stack + CSL_STACK_SIZE)
        : "x1", "memory"
    );
};
