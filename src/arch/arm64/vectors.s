/* src/arch/arm64/vectors.s */

.section .text
.align 11   // vector table must be 2KB (0x800) aligned
.global vector_table
.global write_vbar_el2

vector_table:
    /* --- Current EL, SP0 --- */
    .align 7
    sync_sp0:      b vec_common
    .align 7
    irq_sp0:       b vec_common
    .align 7
    fiq_sp0:       b vec_common
    .align 7
    serr_sp0:      b vec_common

    /* --- Current EL, SPx (this is the one you'll actually hit, EL2->EL2) --- */
    .align 7
    sync_spx:      mov x4, #0; b vec_common
    .align 7
    irq_spx:       mov x4, #1; b vec_common
    .align 7
    fiq_spx:       mov x4, #2; b vec_common
    .align 7
    serr_spx:      mov x4, #3; b vec_common

    /* --- Lower EL, AArch64 --- */
    .align 7
    sync_lower64:  b vec_common
    .align 7
    irq_lower64:   b vec_common
    .align 7
    fiq_lower64:   b vec_common
    .align 7
    serr_lower64:  b vec_common

    /* --- Lower EL, AArch32 --- */
    .align 7
    sync_lower32:  b vec_common
    .align 7
    irq_lower32:   b vec_common
    .align 7
    fiq_lower32:   b vec_common
    .align 7
    serr_lower32:  b vec_common

    .align 7

vec_common:
    // Save enough state to be useful, then call into C
    sub sp, sp, #256
    stp x0, x1, [sp, #0]
    stp x2, x3, [sp, #16]
    // ... (save whatever regs your dump function needs; x0-x30 if you want full state)

    mrs x0, esr_el2
    mrs x1, far_el2
    mrs x2, elr_el2
    mrs x3, spsr_el2

    bl exception_dump   // your existing C print logic, refactored to take these as args



halt:
    wfi
    b halt

write_vbar_el2:
    msr vbar_el2, x0
    isb
    ret