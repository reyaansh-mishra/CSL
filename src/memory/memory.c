/* src/memory/memory.c s*/

#include <stdbool.h>

#include <core.h>
#include <memory/block_alloc.h>
#include <memory/maymory.h>
#include <SYSTEM_STATE.h>

PAGES malloc(size_t pages) {
    switch ((int)uefi_died) {
        case true:
            return (PAGES){.start = block_alloc(pages*PAGE_SIZE), .num_of_pages = pages};
        case false:
            return (PAGES){.start = uefi_provider(pages*PAGE_SIZE), .num_of_pages = pages};
    };
    return (PAGES){NULL, 0};
};

void free(PAGES pages) {
    block_free(pages.start);
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

