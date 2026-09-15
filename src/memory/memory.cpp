/* src/memory/memory.cpp */

#include <utils.hpp>
#include <specific-includes/block_allocator.hpp>

extern "C" {
    #include <specific-includes/terminal.h>
    #include <specific-includes/memory.h>
    #include <specific-includes/bootstrappr.h>
    #include <payload-includes/payload.h>
    #include <mmu.h>
};

#define BLCK_ALLOCATOR_INIT_PAGES   3000

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

extern "C" void* memcpy(void* destination, const void* source, size_t size) {
    uint8_t*          dest = (uint8_t* )destination;
    const uint8_t*    src =  (const uint8_t* )source;

    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
    };
    return dest;
};

extern "C" void* memset(void* dest, int val, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    for (size_t i = 0; i < n; i++) {
        d[i] = (uint8_t)val;
    }
    return dest;
}

extern "C" void* memmove(void* dest, const void* src, size_t n) {
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
    uintptr_t offset = (uintptr_t)&csl_continue_if_needed - efi.csl_base;
    uint64_t csl_base_old = efi.csl_base;
    efi.csl_base = addr;
    // INFO("Died in memcpy..?\n");
    memcpy((void*)addr, (void*)csl_base_old, efi.csl_size);
    
    INFO("!!!JUMPING!!!\n");
    // INFO("Died in br..? EXTRA INFO: last_addr = %p, offset = %p\n", addr, offset);
    
    __asm__ volatile("dsb sy");
    __asm__ volatile("isb");
    __asm__ volatile(
        "br %0"
        :
        : "r"(addr + offset)
        : "memory"
    );
    __builtin_unreachable();
};

void enable_malloc() {
    INFO("ENABLING CSL BLOCK ALLOCATOR WITH NUMBER OF PAGES: %d\n", BLCK_ALLOCATOR_INIT_PAGES);
    uintptr_t block = (uintptr_t)alloc_pages(BLCK_ALLOCATOR_INIT_PAGES, EfiLoaderData);

    for (size_t i = 0; i < BLCK_ALLOCATOR_INIT_PAGES; i++) {
        setup_table_for_page(block + i*CSL_PAGE_SIZE, block + i*CSL_PAGE_SIZE, (enum VIRT_ADDR_PERMISSIONS)(WRITABLE | EXECUTABLE));
    }
    allocator.init((void* )block, BLCK_ALLOCATOR_INIT_PAGES);
    pls_use_malloc_now = true;
};
