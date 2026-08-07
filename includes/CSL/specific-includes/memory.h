/* includes/CSL/memory.hpp */

#pragma once
#include <csl.h>

struct MemMapprInfo {
    EFI_MEMORY_DESCRIPTOR*  memory_map;
    UINTN                   memory_map_size;
    UINTN                   map_key;
    UINTN                   descriptor_size;
    UINT32                  descriptor_version;
};

int                 mem_map_init();
void*               mem_alloc(size_t size, EFI_MEMORY_TYPE memory_type);
void*               alloc_page();
void*               alloc_pages(UINTN num_pages, EFI_MEMORY_TYPE memory_type);
struct MemMapprInfo getMemMap();
void*               memcpy(void* dest, const void* src, size_t n);
void*               memmove(void* dest, const void* src, size_t n);
void*               memset(void* dest, int val, size_t n);
void                move_csl_to_addr(uintptr_t last_addr);
void                setup_stack_with_top_addr(uintptr_t top_addr);
void                setup_stack();

bool is1GbAligned(uintptr_t ramBase);
