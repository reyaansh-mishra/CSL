/* includes/CSL/memory.h */

#pragma once
#include <csl.h>
#include <uefi/mem-map.h>

int                 mem_map_init();
void*               mem_alloc(size_t size, EFI_MEMORY_TYPE memory_type);
void*               alloc_page();
void*               alloc_pages(UINTN num_pages, EFI_MEMORY_TYPE memory_type);
UEFI_MEMORY_MAP     getMemMap();
void*               memcpy(void* dest, const void* src, size_t n);
void*               memmove(void* dest, const void* src, size_t n);
void*               memset(void* dest, int val, size_t n);
void                move_csl_to_addr(uintptr_t addr);
void                enable_malloc();
bool                is1GbAligned(uintptr_t ramBase);

void* malloc(size_t pages);
void  free(void* ptr);
