#include <specific-includes/memory.h>
#define STACK_SIZE  0x10000

void setup_stack()
{
    uintptr_t stack_bottom  = (uintptr_t)mem_alloc(STACK_SIZE, EfiLoaderData);
    uintptr_t stack_top     = stack_bottom + STACK_SIZE;

    setup_stack_with_top_addr(stack_top);
};
