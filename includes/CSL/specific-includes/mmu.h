/* includes/CSL/specific-includes/mmu.h */

#include <csl.h>
#include <payload-includes/payload.h>

void start_mmu_work();
void setup_table_for_page(uintptr_t physical, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions);
