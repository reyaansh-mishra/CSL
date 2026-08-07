/* src/mmu/descriptors/l3.cpp */

extern "C" {
    #include <terminal.h>
};

#include <utils.hpp>
#include <mmu.hpp>
#include <page_descriptor_helper.hpp>

#define PHY_PAGE_ADDR_TOP   49
#define PHY_PAGE_ADDR_BOT   12

/* ----------------------------------------------------------------------- */
/* HELPERS */
/* ----------------------------------------------------------------------- */

bool Page_Descriptor::is_valid() const
{
    return (get_bit(raw, 1) && get_bit(raw, 0));
};

void Page_Descriptor::clear() { raw = 0; };

void Page_Descriptor::setup_L3_table(struct L3_Page_Descriptor_Info minimal_table_info)
{
    raw |= 
          (minimal_table_info.mair_index_info << 2)
        | (minimal_table_info.rw_info  << 6)   // AP[2], RO bit
        | (minimal_table_info.share_info << 8)
        | (minimal_table_info.access_info << 10)
        | (minimal_table_info.ng_info << 11)
        | (minimal_table_info.gp_info << 50)
        | (minimal_table_info.dbm_info << 51)
        | (minimal_table_info.contiguous_info << 52)
        | (minimal_table_info.priv_exec_info << 53)
        | (minimal_table_info.exec_info << 54)
        | (0 << 5);   // NS, RES0, Explicit
};

void Page_Descriptor::set_valid(bool state)
{
    set_bit(raw, 0, state);
};


/* ----------------------------------------------------------------------- */
/* CORE FUNCTIONS */
/* ----------------------------------------------------------------------- */

void Page_Descriptor::init()
{
    raw = 0;
};

void Page_Descriptor::validate(struct L3_Page_Descriptor_Info table, uintptr_t phy_addr)    // One-shot
{

    ASSERT((phy_addr & 0xFFF) == 0);

    raw = 0;

    set_bit(raw,0,1);
    set_bit(raw,1,1);
    // for (size_t i = 10; i < 12; i++)  { set_bit(raw, i, 1); };
    // for (size_t i = 5;  i < 8; i++)   { set_bit(raw, i, 0); };

    for (size_t i = PHY_PAGE_ADDR_BOT; i < PHY_PAGE_ADDR_TOP+1; i++) { // Include PHY_PAGE_ADDR_TOP.
        set_bit(raw, i, get_bit(phy_addr, i));
    };

    setup_L3_table(table);
};

uintptr_t Page_Descriptor::get_page_addr()
{
    uintptr_t final = 0;

    for (size_t i = PHY_PAGE_ADDR_BOT; i < PHY_PAGE_ADDR_TOP+1; i++) {
        set_bit(final, i, get_bit(raw, i));
    };

    return final;
}; 
