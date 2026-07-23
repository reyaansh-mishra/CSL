.section .text
.global mask_interrupts
.global unmask_interrupts
.global mask_FULL

mask_interrupts:
    msr     daifset, #3       // #3 sets both 'I' (IRQ) and 'F' (FIQ) bits
    msr     daifset, #2   /* Sets Bit 7 (I) -> Masks IRQ again, just to be safe. */
    isb
    ret

unmask_interrupts:
    msr     daifclr, #3       // Clears both 'I' and 'F' bits
    ret

mask_FULL:
    msr daifset, #0xf  /* Masks Debug, SError, IRQ, and FIQ */
    isb
    ret