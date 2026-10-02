/* src/memory/memory.c s*/

#include <stdbool.h>

#include <core.h>
#include <memory/block_alloc.h>
#include <memory/maymory.h>
#include <SYSTEM_STATE.h>
#include <stddef.h>
#include <stdint.h>

PAGES malloc(size_t pages) {
    switch ((int)uefi_died) {
        case true:
            return (PAGES){.start = block_alloc(pages*PAGE_SIZE), .num_of_pages = pages};
        case false:
            return (PAGES){.start = uefi_malloc_provider(pages*PAGE_SIZE), .num_of_pages = pages};
    };
    return (PAGES){0};
};

void free(PAGES pages) {
    switch ((int)uefi_died) {
        case true:
            block_free(pages.start);
        case false:
            uefi_free_provider(pages.start);
    };
};

void* memcpy(void* destination, const void* source, size_t size) {
    uint8_t*          dest = (uint8_t* )destination;
    const uint8_t*    src =  (const uint8_t* )source;

    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
    };
    return dest;
};

void* memset(void* dest, int val, size_t size) {
    uint8_t* destination = dest;

    for (size_t i = 0; i < size; i++) {
        destination[i] = (uint8_t)val;
    };
    return dest;
};

void* memmove(void* dest, const void* src, size_t size) {
    uint8_t* destination    = dest;
    const uint8_t* source   = src;

    if (destination < source) {
        for (size_t i = 0; i < size; i++) destination[i] = source[i];
    } else {
        for (size_t i = size; i > 0; i--) destination[i] = source[i];
    };
    return dest;
};

