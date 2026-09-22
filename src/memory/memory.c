/* src/memory/memory.c s*/

#include <core.h>
#include <memory/block_alloc.h>
#include <memory/maymory.h>
#include <SYSTEM_STATE.h>

PAGES malloc(size_t pages) {
    if (uefi_died)
        return (PAGES){.start = block_alloc(pages*PAGE_SIZE), .num_of_pages = pages};
    else
        return (PAGES){.start = uefi_provider(pages*PAGE_SIZE), .num_of_pages = pages};
};

void free(PAGES pages) {
    block_free(pages.start);
};
