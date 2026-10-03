/* src/mmu/Translation_Tables/tt_handler.cpp */

extern "C" {
    #include <mmu.h>
    #include <memory.h>
    #include <terminal.h>
    #include <aarch64.h>
    #include <mmu/orchestrator_required.h>
    #include <utils.h>
};
#include <mmu/page_descriptor_helper.hpp>
#include <mmu/page_and_table_descriptor.hpp>

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

static void setup_l2(uintptr_t phy, uintptr_t virt, PAGE_PERMS perms) {
    uintptr_t           l0_bits     = get_bits(virt, 47, 39);
    uintptr_t           l1_bits     = get_bits(virt, 38, 30);
    uintptr_t           l2_bits     = get_bits(virt, 29, 21);

    uintptr_t           l2_table    = (uintptr_t)malloc(1).start;

    Table_Descriptor*   L0          = &L0_table[l0_bits];
    Table_Descriptor*   L1          = NULL;
    L2_final*           L2          = NULL;

    if (!L0->is_active()) setup_l1(virt, l2_table);
    L1 = &((Table_Descriptor *)L0->get_next_table())[l1_bits];
    
    if (!L1->is_active()) setup_l1(virt, l2_table);
    L2 = &((L2_final *)L1->get_next_table())[l2_bits];

    bool consumed = ((uintptr_t)L1->get_next_table() == l2_table);

    if (!L2->is_active()) {
        L2->init();
        L2->activate(setup_perms(perms), phy);
    } else {
        if (phy == L2->get_page()) {   // OK
            INFO("L2 TABLE ALREADY Exists!\n");
        } else {
            ERR("L2 TABLE EXISTS but with DIFFERENT Next TABLE! next_table: %p, Actual Next Table: %p\n", phy, L2->get_page());
        }
    };

    if (!consumed) free({l2_table, 1});
};

extern "C" void setup_table_for_2MB(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions) {
    ASSERT((virt & 0x1FFFFF) == 0);
    ASSERT((phy  & 0x1FFFFF) == 0); // dbg behaviour for now
    virt = round_down(virt, 0x200000);
    phy  = round_down(phy, 0x200000);
    
    setup_l2(phy, virt, permissions);
};

