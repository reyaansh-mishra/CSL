.section .text

.global _start

_start:

    mrs x0, CurrentEL
    lsr x0, x0, #2
    cmp x0, #2
    b.eq efi_main
    b   not_el2_halt      // or a print+spin, or an attempt to drop EL3->EL2

    not_el2_halt:
        b not_in_el2
        b not_el2_halt
