#include <utils.h>
#include <core.h>
#include <mmu.h>
#include <terminal.h>
#include <mmu/orchestrator_required.h>

static void map_for_page_only(PAGES phy, PAGES virt, PAGE_PERMS perms) {
    for (size_t i = 0; i < phy.num_of_pages; i++) {
        setup_table_for_page(phy.start + i*PAGE_SIZE, virt.start + i*PAGE_SIZE, perms);
    };
};

void map_pages(PAGES phy, PAGES virt, PAGE_PERMS perms) {
    ASSERT(phy.num_of_pages == virt.num_of_pages);

    if (phy.num_of_pages < 262144)
        map_for_page_only(phy, virt, perms);
    else if (phy.num_of_pages >= 262144)
     ;
};
