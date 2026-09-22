/* private-includes/memory/block_alloc.h */

#include <stdint.h>
#include <stddef.h>

uintptr_t   block_alloc(size_t size);
void        block_free(uintptr_t ptr);
