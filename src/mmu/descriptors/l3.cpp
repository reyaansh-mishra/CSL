/* src/mmu/descriptors/l3.cpp */

#include <utils.hpp>
#define PHY_PAGE_ADDR_TOP   49
#define PHY_PAGE_ADDR_BOT   12

/** INFO: 
 * [63:59]  -> PAGE BASED HWD ATTRIBUTES
 * [58:55]  -> SOFTWARE DEFINED FLAGS (IGNORED)
 * [54]     -> UXN              — Unprivileged Execute Never (EL0 cannot execute)
 * [53]     -> PXN              — Privileged Execute Never (EL1 cannot execute)
 * [52]     -> CONTIGUOUS HINT  — TLB can merge contiguous entries  (IGNORE)
 * [49:12]  -> PHYSICAL PAGE ADDR (from addr -> [49:12])
 * [11:10]  -> SET TO 1 (Access Flag && Not-Global)
 * [9:8]    -> Shareability (from addr -> [1:0])
 * [7:5]    -> SET TO 0 (Access Permissions && Non-Secure)
 * [1]      -> TYPE (set 1)
 * [0]      -> VALIDITY
 */

/* ----------------------------------------------------------------------- */
/* HELPERS */
/* ----------------------------------------------------------------------- */

bool Page_Descriptor::is_valid() const
{
    return (get_bit(raw, 1) && get_bit(raw, 0));
};

void Page_Descriptor::clear() { raw = 0; };

void Page_Descriptor::validate()    // One-shot
{
    set_bit(raw, 0, 1);
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

    for (size_t i = 10; i < 12; i++)    { set_bit(raw, i, 1); };
    for (size_t i = 5; i < 8; i++)      { set_bit(raw, i, 0); };

    set_bit(raw, 1, 1);
};

void Page_Descriptor::set_page_addr(uintptr_t phy_addr)
{
    ASSERT((phy_addr & 0xFFF) == 0);

    for (size_t i = PHY_PAGE_ADDR_BOT; i < PHY_PAGE_ADDR_TOP+1; i++) { // Include PHY_PAGE_ADDR_TOP.
        set_bit(raw, i, get_bit(phy_addr, i));
    };
};
