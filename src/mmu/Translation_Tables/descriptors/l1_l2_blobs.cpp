/* src/mmu/Translation_Tables/descriptors/l1_l2_blobs.cpp */

#include <utils.hpp>
#include <mmu/page_and_table_descriptor.hpp>
#include <mmu/page_descriptor_helper.hpp>

extern "C" {
    #include <terminal.h>
};

void L2_final::activate(struct Descriptor_Info table, uintptr_t phy_addr) {
    Page_Descriptor::activate(table, phy_addr);
    set_bit(raw, 1, 0);
};

void L1_final::activate(struct Descriptor_Info table, uintptr_t phy_addr) {
    Page_Descriptor::activate(table, phy_addr);
    set_bit(raw, 1, 0);
}; 
