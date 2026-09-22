/* includes/mmu.h */

#include <core.h>

void map_pages(PAGES phy, PAGES virt);   // ALSO generates
                                        // Respective TT

void fixup_mmu();   // DISABLES MMU
