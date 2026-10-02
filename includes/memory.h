/* includes/memory.h */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <core.h>

typedef uintptr_t (*memory_provider_alloc_t)(size_t size);
typedef uintptr_t (*memory_provider_free_t)(uintptr_t ptr);

PAGES   malloc(size_t pages);
void    free(PAGES pages);

void set_provider_for_uefi(memory_provider_alloc_t addr_of_func_to_call);   // EXACTLY of the signature of: <name>(size in size_t)
