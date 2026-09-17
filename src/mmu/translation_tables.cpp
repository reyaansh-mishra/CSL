/* src/mmu/mmu.cpp */

extern "C" {
    #include <mmu/orchestrator_required.h>
    #include <memory.h>
    #include <terminal.h>
    #include <arm64.h>
};

#include "mmu/page_descriptor_helper.hpp"
#include <utils.hpp>
#include <mmu/page_and_table_descriptor.hpp>

Table_Descriptor __attribute__((aligned(CSL_PAGE_SIZE))) L0_table[512];

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <mmu internals::setup perms>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu internals::setup perms>: " fmt, ##__VA_ARGS__)

static struct L3_Page_Descriptor_Info setup_perms(enum VIRT_ADDR_PERMISSIONS __attribute__((unused)) perms) {
    help_me_build_page_entry page;
    page.set_default_values();
    if (perms & WRITABLE) {
        page.set_rw_perms(EL2_RW);
    } else {
        page.set_rw_perms(EL2_RO);
    };

    if (perms & EXECUTABLE)
        page.set_exec(EXEC_AVAIL);
    else page.set_exec(EXEC_UNAVAIL);

    if (perms & DEVICE)
        page.set_mair(ATTR_IDX_0);
    else page.set_mair(ATTR_IDX_1);

    return page.get();
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <mmu internals::L0>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu internals::L0>: " fmt, ##__VA_ARGS__)

void setup_l0_entry(uintptr_t virt, uintptr_t next_table) {
    uintptr_t           l0_bits = get_bits(virt, 47, 39);
    Table_Descriptor*   L0      = &L0_table[l0_bits];
    if (!L0->is_valid()) {  // Setup L0 table
        L0->init();
        L0->validate();
        L0->set_next_table(next_table);
    } else {
        if (next_table == L0->get_next_level()) {   // OK
            INFO("L0 Table ALREADY Exists!\n");
        } else {
            ERR("L0 Table EXISTS but with DIFFERENT Next Table! next_table: %p, Actual Next table: %p\n", next_table, L0->get_next_level());
        }
    }
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <mmu internals::L1>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu internals::L1>: " fmt, ##__VA_ARGS__)

void setup_l1_entry(uintptr_t virt, uintptr_t next_table) {
    uintptr_t           l0_bits     = get_bits(virt, 47, 39);
    uintptr_t           l1_bits     = get_bits(virt, 38, 30);

    uintptr_t           l1_table    = (uintptr_t)alloc_page();  // ACCEPTABLE if Leaked FOR NOW.
    Table_Descriptor*   L1_table    = NULL;

    Table_Descriptor*   L0          = &L0_table[l0_bits];
    Table_Descriptor*   L1          = NULL;

    if (!L0->is_valid()) {
        setup_l0_entry(virt, l1_table);
        L0 = &L0_table[l0_bits];
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];
    } else {
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];
    };

    if (!L1->is_valid()) {
        L1->init();
        L1->set_next_table(next_table);
        L1->validate();
    } else {
        if (next_table == L1->get_next_level()) {   // OK
            INFO("L1 Table ALREADY Exists!\n");
        } else {
            ERR("L1 Table EXISTS but with DIFFERENT Next Table! next_table: %p, Actual Next table: %p\n", next_table, L1->get_next_level());
        }
        free((void* )l1_table);
    }
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <mmu internals::L2>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu internals::L2>: " fmt, ##__VA_ARGS__)

void setup_l2_entry(uintptr_t virt, uintptr_t next_table) {
    uintptr_t           l0_bits     = get_bits(virt, 47, 39);
    uintptr_t           l1_bits     = get_bits(virt, 38, 30);
    uintptr_t           l2_bits     = get_bits(virt, 29, 21);

    uintptr_t           l2_table    = (uintptr_t)alloc_page();  // ACCEPTABLE if Leaked FOR NOW.
    Table_Descriptor*   L1_table    = NULL;
    Table_Descriptor*   L2_table    = NULL;

    Table_Descriptor*   L0          = &L0_table[l0_bits];
    Table_Descriptor*   L1          = NULL;
    Table_Descriptor*   L2          = NULL;

    if (!L0->is_valid()) {
        setup_l1_entry(virt, l2_table);
        L0 = &L0_table[l0_bits];
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];

        L2_table = (Table_Descriptor* )L1->get_next_level();
        L2       = (Table_Descriptor* )&L2_table[l2_bits];

    } else {
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];
    };

    if (!L1->is_valid()) {
        setup_l1_entry(virt, l2_table);
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];

        L2_table = (Table_Descriptor* )L1->get_next_level();
        L2       = (Table_Descriptor* )&L2_table[l2_bits];
    } else {
        L2_table = (Table_Descriptor* )L1->get_next_level();
        L2       = (Table_Descriptor* )&L2_table[l2_bits];
    };

    if (!L2->is_valid()) {
        L2->init();
        L2->set_next_table(next_table);
        L2->validate();
    } else {
        if (next_table == L2->get_next_level()) {   // OK
            INFO("L2 Table ALREADY Exists!\n");
        } else {
            ERR("L2 Table EXISTS but with DIFFERENT Next Table! next_table: %p, Actual Next table: %p\n", next_table, L2->get_next_level());
        }
        free((void* )l2_table);
    }
};

#undef  INFO
#undef  ERR
#define INFO(fmt, ...)  print("[CSL] <mmu internals::L3>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu internals::L3>: " fmt, ##__VA_ARGS__)

void setup_l3_entry(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS perms) {
    uintptr_t           l0_bits     = get_bits(virt, 47, 39);
    uintptr_t           l1_bits     = get_bits(virt, 38, 30);
    uintptr_t           l2_bits     = get_bits(virt, 29, 21);
    uintptr_t           l3_bits     = get_bits(virt, 20, 12);

    uintptr_t           l3_table    = (uintptr_t)alloc_page();  // ACCEPTABLE if Leaked FOR NOW.
    Table_Descriptor*   L1_table    = NULL;
    Table_Descriptor*   L2_table    = NULL;
    Page_Descriptor*    L3_table    = NULL;

    Table_Descriptor*   L0          = &L0_table[l0_bits];
    Table_Descriptor*   L1          = NULL;
    Table_Descriptor*   L2          = NULL;
    Page_Descriptor*    L3          = NULL;


    if (!L0->is_valid()) {
        setup_l2_entry(virt, l3_table);
        L0 = &L0_table[l0_bits];
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];

        L2_table = (Table_Descriptor* )L1->get_next_level();
        L2       = (Table_Descriptor* )&L2_table[l2_bits];

        L3_table = (Page_Descriptor*  )L2->get_next_level();
        L3       = (Page_Descriptor* )&L3_table[l3_bits];
    } else {
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];
    };

    if (!L1->is_valid()) {
        setup_l2_entry(virt, l3_table);
        L1_table = (Table_Descriptor* )L0->get_next_level();
        L1       = (Table_Descriptor* )&L1_table[l1_bits];

        L2_table = (Table_Descriptor* )L1->get_next_level();
        L2       = (Table_Descriptor* )&L2_table[l2_bits];

        L3_table = (Page_Descriptor*  )L2->get_next_level();
        L3       = (Page_Descriptor* )&L3_table[l3_bits];
    } else {
        L2_table = (Table_Descriptor* )L1->get_next_level();
        L2       = (Table_Descriptor* )&L2_table[l2_bits];
    };

    if (!L2->is_valid()) {
        setup_l2_entry(virt, l3_table);
        L2_table = (Table_Descriptor* )L1->get_next_level();
        L2       = (Table_Descriptor* )&L2_table[l2_bits];

        L3_table = (Page_Descriptor*  )L2->get_next_level();
        L3       = (Page_Descriptor* )&L3_table[l3_bits];
    } else {
        L3_table = (Page_Descriptor*  )L2->get_next_level();
        L3       = (Page_Descriptor* )&L3_table[l3_bits];
    };
    if (!L3->is_valid()) {
        L3->init();
        L3->validate(setup_perms(perms), phy);
    } else {
        if (phy == L3->get_page_addr()) {   // OK
            INFO("L3 PAGE ALREADY Exists!\n");
        } else {
            ERR("L3 PAGE EXISTS but with DIFFERENT Next Page! l3_page: %p, Actual Next Page: %p\n", phy, L3->get_page_addr());
        }
        free((void* )l3_table);
    }
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <mmu internals::setup page>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu internals::setup page>: " fmt, ##__VA_ARGS__)

void setup_table_for_page(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions) {
    setup_l3_entry(phy, virt, permissions);
};

// Putting here becuase why not

/* AI GENERATED */
uint64_t make_tcr()
{
    uint64_t tcr = 0;

    tcr |= (16ULL << 0);       // T0SZ  [5:0]   = 16 (48-bit Virtual Address space)
    tcr |= (0b01ULL << 8);     // IRGN0 [9:8]
    tcr |= (0b01ULL << 10);    // ORGN0 [11:10]
    tcr |= (0b11ULL << 12);    // SH0   [13:12]
    tcr |= (0b00ULL << 14);    // TG0   [15:14] = 4KB
    tcr |= (0b010ULL << 16);   // PS    [18:16] = 40-bit

    tcr |= (1ULL << 23);       // RES1 — REQUIRED for TCR_EL2 (E2H=0)
    tcr |= (1ULL << 31);       // RES1 — REQUIRED for TCR_EL2 (E2H=0)

    return tcr;
}

uint64_t make_mair()
{
    uint64_t mair = 0;

    mair |= (0x00ULL << 0);  // Attr0 = Device-nGnRnE
    mair |= (0xFFULL << 8);  // Attr1 = Normal WB cacheable

    return mair;
};
/* END AI GENERATED */

void mmu_bs() {
    INFO("Disabling Virtualization!\n");    // UEFI Dropped us into Virtualized.
    uint64_t hcr = read_hcr();
    hcr &= ~(1ULL << 34);   // clear E2H
    hcr &= ~(1ULL << 27);   // clear TGE
    write_hcr(hcr);

    install_vbar();
    disable_mmu();
    write_ttbr0((uint64_t)&L0_table[0]);
    write_tcr(make_tcr());
    write_mair(make_mair());

    enable_mmu();
};
