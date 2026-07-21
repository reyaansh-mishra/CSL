/* src/memory/memory.cpp */

#include <utils.hpp>

void* mem_alloc(size_t size, EFI_MEMORY_TYPE memory_type) {
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
    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS addr = 0;

    status = efi.SystemTable->BootServices->AllocatePages(
        AllocateAnyPages,
        memory_type,
        num_pages,
        &addr
    );

    if (EFI_ERROR(status)) {
        ERR("MemBasicOps.cpp: mem_alloc_pages: AllocatePages failed with Code: ");
        print((uint64_t)status);
        print("\n");
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

extern "C" void* memcpy(void* dest, const void* src, size_t n) {
    uint8_t* d = (uint8_t*)dest;
    const uint8_t* s = (const uint8_t*)src;
    for (size_t i = 0; i < n; i++) {
        d[i] = s[i];
    }
    return dest;
}

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
