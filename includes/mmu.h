/* includes/mmu.h */

#pragma once
#include <core.h>

enum VIRT_ADDR_PERMISSIONS {
    READ_ONLY   = 1 << 0,
    WRITABLE    = 1 << 1,
    EXECUTABLE  = 1 << 2,

    DEVICE      = 1 << 3,
};
typedef enum VIRT_ADDR_PERMISSIONS PAGE_PERMS;


void map_pages(PAGES phy, PAGES virt, PAGE_PERMS perms);    // ALSO generates
                                                            // Respective TT

void setup_mmu();   // DISABLES MMU
