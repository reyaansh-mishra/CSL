#include <mmu.h>

void setup_table_for_page(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions);
void setup_table_for_2MB(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions);
void setup_table_for_1GB(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions);
