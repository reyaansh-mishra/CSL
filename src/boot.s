.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

/*
The linker script specifies _start as the entry point to the kernel and the
bootloader will jump to this position once the kernel has been loaded. It
doesn't make sense to return from this function as the bootloader is gone.
*/
.section .text

.global _start
.type _start, @function
_start:
    ldr x30, =stack_top
    mov sp, x30
    bl csl_main
    b .