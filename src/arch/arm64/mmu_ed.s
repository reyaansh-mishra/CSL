/* src/arch/arm64/mmu_ed.s */

.section .text
.global disable_mmu
.global enable_mmu
.global write_ttbr0_el2
.global write_tcr_el2
.global write_mair_el2

disable_mmu:
    // 1. Read SCTLR_EL2 into X0
    mrs x0, sctlr_el2

    // 2. Clear the M bit (bit 0)
    bic x0, x0, #1 // #1 corresponds to (1 << 0)

    // 3. Write back to SCTLR_EL2
    msr sctlr_el2, x0

    // 4. Execute an ISB instruction to ensure the system register update takes effect
    isb

    ret


enable_mmu:
    mrs x0, sctlr_el2
    orr x0, x0, #1
    msr sctlr_el2, x0
    isb
    ret


write_ttbr0_el2:
    msr ttbr0_el2, x0
    isb
    ret

write_tcr_el2:
    msr tcr_el2, x0
    isb
    ret

write_mair_el2:
    msr mair_el2, x0
    isb
    ret