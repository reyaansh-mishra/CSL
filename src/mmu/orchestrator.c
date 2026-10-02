/* src/mmu/orchestrator.c */

#include <stdbool.h>

#include <core.h>
#include <utils.h>
#include <terminal.h>
#include <mmu.h>
#include <mmu/orchestrator_required.h>


#define PAGES_FOR_2MB   512ULL
#define PAGES_FOR_1GB   262144ULL

static inline bool is_block_aligned(uintptr_t addr, size_t pages) {
    return (addr & (pages * PAGE_SIZE - 1)) == 0;
}

/* AI */

void map_pages(PAGES phy, PAGES virt, PAGE_PERMS perms) {
    ASSERT(phy.num_of_pages == virt.num_of_pages);

    uintptr_t p    = phy.start;
    uintptr_t v    = virt.start;
    size_t    left = phy.num_of_pages;

    while (left) {
        size_t step;

        if (left >= PAGES_FOR_1GB &&
            is_block_aligned(p, PAGES_FOR_1GB) && is_block_aligned(v, PAGES_FOR_1GB)) {
            setup_table_for_1GB(p, v, perms);
            step = PAGES_FOR_1GB;
        } else if (left >= PAGES_FOR_2MB &&
                   is_block_aligned(p, PAGES_FOR_2MB) && is_block_aligned(v, PAGES_FOR_2MB)) {
            setup_table_for_2MB(p, v, perms);
            step = PAGES_FOR_2MB;
        } else {
            setup_table_for_page(p, v, perms);
            step = 1;
        }

        p    += step * PAGE_SIZE;
        v    += step * PAGE_SIZE;
        left -= step;
    }
}
