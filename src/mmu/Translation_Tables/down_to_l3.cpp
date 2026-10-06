/* src/mmu/Translation_Tables/tt_handler.cpp */

extern "C" {
    #include <mmu.h>
    #include <memory.h>
    #include <terminal.h>
    #include <aarch64.h>
    #include <mmu/orchestrator_required.h>
    #include <SYSTEM_STATE.h>
};
#include <mmu/page_descriptor_helper.hpp>
#include <mmu/page_and_table_descriptor.hpp>

Table_Descriptor __attribute__((aligned(PAGE_SIZE))) L0_table[512];

#undef INFO
#undef ERR
#define INFO(fmt, ...)  printf("[CSL] <mmu internals::setup perms>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <mmu internals::setup perms>: " fmt, ##__VA_ARGS__)

static struct Descriptor_Info setup_perms(PAGE_PERMS perms) {
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
#define INFO(fmt, ...)  printf("[CSL] <mmu internals::L0>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <mmu internals::L0>: " fmt, ##__VA_ARGS__)

static void setup_l0(uintptr_t virt, uintptr_t next_table) {
    uintptr_t           l0_bits = get_bits(virt, 47, 39);
    Table_Descriptor*   L0      = &L0_table[l0_bits];
    if (!L0->is_active()) {  // Setup L0 table
        L0->init();
        L0->activate(next_table);
    } else {
        if (next_table == L0->get_next_table()) {   // OK
            INFO("L0 Table ALREADY Exists!\n");
        } else {
            ERR("L0 Table EXISTS but with DIFFERENT Next Table! next_table: %p, Actual Next table: %p\n", next_table, L0->get_next_table());
        }
    }
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  printf("[CSL] <mmu internals::L1>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <mmu internals::L1>: " fmt, ##__VA_ARGS__)

static void setup_l1(uintptr_t virt, uintptr_t next_table) {
    uintptr_t           l0_bits     = get_bits(virt, 47, 39);
    uintptr_t           l1_bits     = get_bits(virt, 38, 30);

    uintptr_t           l1_table    = (uintptr_t)malloc(1).start;

    Table_Descriptor*   L0          = &L0_table[l0_bits];
    Table_Descriptor*   L1          = NULL;

    if (!L0->is_active()) setup_l0(virt, l1_table);
    L1 = &((Table_Descriptor *)L0->get_next_table())[l1_bits];
    
    bool consumed = ((uintptr_t)L0->get_next_table() == l1_table);

    if (!L1->is_active()) {
        L1->init();
        L1->activate(next_table);
    } else {
        if (next_table == L1->get_next_table()) {   // OK
            INFO("L1 TABLE ALREADY Exists!\n");
        } else {
            ERR("L1 TABLE EXISTS but with DIFFERENT Next TABLE! next_table: %p, Actual Next Table: %p\n", next_table, L1->get_next_table());
        }
    };

    if (!consumed) free({l1_table, 1});
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  printf("[CSL] <mmu internals::L2>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <mmu internals::L2>: " fmt, ##__VA_ARGS__)

static void setup_l2(uintptr_t virt, uintptr_t next_table) {
    uintptr_t           l0_bits     = get_bits(virt, 47, 39);
    uintptr_t           l1_bits     = get_bits(virt, 38, 30);
    uintptr_t           l2_bits     = get_bits(virt, 29, 21);

    uintptr_t           l2_table    = (uintptr_t)malloc(1).start;

    Table_Descriptor*   L0          = &L0_table[l0_bits];
    Table_Descriptor*   L1          = NULL;
    Table_Descriptor*   L2          = NULL;

    if (!L0->is_active()) setup_l1(virt, l2_table);
    L1 = &((Table_Descriptor *)L0->get_next_table())[l1_bits];
    
    if (!L1->is_active()) setup_l1(virt, l2_table);
    L2 = &((Table_Descriptor *)L1->get_next_table())[l2_bits];

    bool consumed = ((uintptr_t)L1->get_next_table() == l2_table);

    if (!L2->is_active()) {
        L2->init();
        L2->activate(next_table);
    } else {
        if (next_table == L2->get_next_table()) {   // OK
            INFO("L2 TABLE ALREADY Exists!\n");
        } else {
            ERR("L2 TABLE EXISTS but with DIFFERENT Next TABLE! next_table: %p, Actual Next Table: %p\n", next_table, L2->get_next_table());
        }
    };

    if (!consumed) free({l2_table, 1});
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  printf("[CSL] <mmu internals::L3>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <mmu internals::L3>: " fmt, ##__VA_ARGS__)

static void setup_l3(uintptr_t phy, uintptr_t virt, PAGE_PERMS perms) {
    uintptr_t           l0_bits     = get_bits(virt, 47, 39);
    uintptr_t           l1_bits     = get_bits(virt, 38, 30);
    uintptr_t           l2_bits     = get_bits(virt, 29, 21);
    uintptr_t           l3_bits     = get_bits(virt, 20, 12);

    uintptr_t           l3_table    = (uintptr_t)malloc(1).start;

    Table_Descriptor*   L0          = &L0_table[l0_bits];
    Table_Descriptor*   L1          = NULL;
    Table_Descriptor*   L2          = NULL;
    Page_Descriptor*    L3          = NULL;

    if (!L0->is_active()) setup_l2(virt, l3_table);
    L1 = &((Table_Descriptor *)L0->get_next_table())[l1_bits];
    
    if (!L1->is_active()) setup_l2(virt, l3_table);
    L2 = &((Table_Descriptor *)L1->get_next_table())[l2_bits];

    if (!L2->is_active()) setup_l2(virt, l3_table);
    L3 = &((Page_Descriptor *)L2->get_next_table())[l3_bits];

    bool consumed = ((uintptr_t)L2->get_next_table() == l3_table);

    if (!L3->is_active()) {
        L3->init();
        L3->activate(setup_perms(perms), phy);
    } else {
        if (phy == L3->get_page()) {   // OK
            INFO("L3 PAGE ALREADY Exists!\n");
        } else {
            ERR("L3 PAGE EXISTS but with DIFFERENT Next Page! l3_page: %p, Actual Next Page: %p\n", phy, L3->get_page());
        }
    };

    if (!consumed) free({l3_table, 1});
};


#undef INFO
#undef ERR
#define INFO(fmt, ...)  printf("[CSL] <mmu internals::setup page>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <mmu internals::setup page>: " fmt, ##__VA_ARGS__)

extern "C" void setup_table_for_page(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions) {
    setup_l3(phy, virt, permissions);
};

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


void setup_mmu() {
    identity_map_csl();
    efi.BootServices->ExitBootServices(efi.ImageHandle, getMemMap().map_key);
    uefi_died = true;
    disable_mmu();
    INFO("Disabling Virtualization!\n");    // UEFI Dropped us into Virtualized.
    uint64_t hcr = read_hcr();
    hcr &= ~(1ULL << 34);   // clear E2H
    hcr &= ~(1ULL << 27);   // clear TGE
    write_hcr(hcr);

    write_ttbr0((uint64_t)&L0_table[0]);
    write_tcr(make_tcr());
    write_mair(make_mair());
    INFO("Enable MMU When Ready!\n");
};
