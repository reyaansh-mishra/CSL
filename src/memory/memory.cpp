/* src/memory/memory.cpp */

extern "C" {
    #include <terminal.h>
    #include <memory.h>
};

#include <utils.hpp>
#include <block_allocator.hpp>
#include <bootstrappr.hpp>


void* mem_alloc(size_t size, EFI_MEMORY_TYPE memory_type) {
    if (pls_use_malloc_now) { return malloc(size/CSL_PAGE_SIZE); };

    EFI_STATUS status;
    void *buffer = NULL;

    status = efi.SystemTable->BootServices->AllocatePool(
        memory_type,
        size,
        &buffer
    );

    if (EFI_ERROR(status)) {
        return NULL;
    }

    return buffer;
};

void* alloc_pages(UINTN num_pages, EFI_MEMORY_TYPE memory_type) {

    if (pls_use_malloc_now) { return malloc(num_pages); };

    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS addr = 0;

    status = efi.SystemTable->BootServices->AllocatePages(
        AllocateAnyPages,
        memory_type,
        num_pages,
        &addr
    );

    if (EFI_ERROR(status)) {
        ERR("memory.cpp: alloc_pages: AllocatePages failed with Code: %lx\n", status);
        return NULL;
    }

    // EFI_PHYSICAL_ADDRESS is a UINT64; AllocatePages guarantees
    // page-aligned (4KB) physical addresses, which is exactly what
    // page tables need.
    return (void*)addr;

};

void* alloc_page() {
    void* page = alloc_pages(1, EfiLoaderData);
    if (page == NULL) {
        return NULL;
    }
    memset(page, 0, CSL_PAGE_SIZE);
    return page;
};

void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

void* memset(void* dest, int val, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    for (size_t i = 0; i < n; i++) {
        d[i] = (uint8_t)val;
    }
    return dest;
}

void* memmove(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    if (d < s) {
        for (size_t i = 0; i < n; i++) d[i] = s[i];
    } else {
        for (size_t i = n; i > 0; i--) d[i-1] = s[i-1];
    }
    return dest;
}

bool is1GbAligned(uintptr_t ramBase) {
    return (ramBase & 0x3FFFFFFF) == 0;
};

void move_csl_to_addr(uintptr_t last_addr)
{
    INFO("Died in memcpy..?\n");
    uintptr_t offset = (uintptr_t)&csl_continue_if_needed - efi.csl_base;
    uint64_t csl_base_old = efi.csl_base;
    efi.csl_base = last_addr;
    memcpy((void*)last_addr, (void*)csl_base_old, efi.csl_size);

    INFO("Died in br..? EXTRA INFO: last_addr = %lx, offset = %lx\n", last_addr, offset);
    
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");
    __asm__ volatile(
        "br %0"
        :
        : "r"(last_addr + offset)
        : "memory"
    );
    __builtin_unreachable();
};
