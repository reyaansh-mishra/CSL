/* src/arch/arm64/mmu_lowlevel.s */

.section .text
.global write_mair
.global write_tcr
.global write_ttbr0
.global enable_mmu
.global disable_mmu

.macro cleanup
    dsb sy  // 1. Data Synchronization Barrier
    isb     // 2. Instruction Synchronization Barrier to flush the pipeline
.endm

write_mair:
    msr mair_el2, x0
    cleanup
    ret

write_tcr:
    msr tcr_el2, x0
    cleanup
    ret

write_ttbr0:
    msr ttbr0_el2, x0
    tlbi alle2is    // 1. Invalidate all EL2 TLB entries (Inner Shareable)
    cleanup
    ret

enable_mmu:
    mrs x0, sctlr_el2
    orr x0, x0, #1
    /* LATER 
    orr x0, x0, #(1 << 0)
    orr x0, x0, #(1 << 2)
    orr x0, x0, #(1 << 12)
    */
    msr sctlr_el2, x0
    cleanup
    ret

disable_mmu:
    mrs x0, sctlr_el2
    bic x0, x0, #1
    msr sctlr_el2, x0
    cleanup
    ret
    