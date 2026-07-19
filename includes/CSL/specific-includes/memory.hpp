#pragma once
#include <csl.h>

struct MemMapprInfo {
    EFI_MEMORY_DESCRIPTOR* memory_map;
    UINTN memory_map_size;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
};

void mem_map_init();

void*               mem_alloc(size_t size, EFI_MEMORY_TYPE memory_type = EfiLoaderData);
void*               alloc_page();
struct MemMapprInfo getMemMap();
extern "C" void*    memcpy(void* dest, const void* src, size_t n);
extern "C" void*    memmove(void* dest, const void* src, size_t n);
extern "C" void*    memset(void* dest, int val, size_t n);

bool is1GbAligned(uintptr_t ramBase);