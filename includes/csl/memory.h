/* includes/memory.h */

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <core.h>
#include <mem_map.h>

typedef void *(*malloc_provider_t)(size_t);
typedef void  (*free_provider_t)(void *);

void* memcpy(void* destination, const void* source, size_t size);
void* memset(void* dest, int val, size_t size);
void* memmove(void* dest, const void* src, size_t size);

PAGES   malloc(size_t pages);
void    free(PAGES pages);

int mem_map_init();
UEFI_MEMORY_MAP getMemMap();
