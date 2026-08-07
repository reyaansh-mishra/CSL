/* src/arch/arm64/stack_control.s */

.section    .text
.global     setup_stack_with_top_addr

setup_stack_with_top_addr:
    mov sp, x0
    ret