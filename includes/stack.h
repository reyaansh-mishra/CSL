/* includes/stack.h */

#include <stdint.h>
#include <core.h>

#define STACK_SIZE PAGE_SIZE*4

void enable_stack(uintptr_t next_func);
