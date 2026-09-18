/* src/memory/memory.cpp */

#include <utils.h>
#include <specific-includes/terminal.h>
#include <specific-includes/memory.h>
#include <specific-includes/bootstrappr.h>
#include <payload-includes/payload.h>
#include <mmu.h>

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <memory>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <memory>: " fmt, ##__VA_ARGS__)

void csl_continue_if_needed();

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
        ERR("ALLOCATION FAILED. ERR: %p\n", status);
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
        ERR("memory.cpp: alloc_pages: AllocatePages failed with Code: %d\n", status);

        while (true) {
            __asm__ volatile("yield");
        };
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

void* memcpy(void* destination, const void* source, size_t size) {
    uint8_t*          dest = (uint8_t* )destination;
    const uint8_t*    src =  (const uint8_t* )source;

    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
    };
    return dest;
};

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

void move_csl_to_addr(uintptr_t addr)
{
    INFO("!!!JUMPING!!!\n");
        
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");
    __asm__ volatile(
        "br %0"
        :
        : "r"(addr)
        : "memory"
    );
    __builtin_unreachable();
};
