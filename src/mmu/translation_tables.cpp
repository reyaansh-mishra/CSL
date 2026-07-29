/* src/mmu/mmu.c */
#include <utils.hpp>

#define TTBR_BASE   &L1_table[0]

/* ----------------------------------------------------------------------- */
/* GLOBALS */
/* ----------------------------------------------------------------------- */

enum VIRT_ADDR_PERMISSIONS {
    NONE        = 1 << 0,   // To be used ONLY FOR INIT. Otherwise will Error out.
    WRITABLE    = 1 << 1,
    EXECUTABLE  = 1 << 2,
};

alignas(4096) Table_Descriptor    L1_table[512];

/* ----------------------------------------------------------------------- */
/* HELPERS */
/* ----------------------------------------------------------------------- */

uintptr_t get_bits_from_ptr(uintptr_t ptr, unsigned start_bit, unsigned stop_bit)   /* from Start -> Stop */
{
    return (ptr >> stop_bit) & ((1ULL << (start_bit - stop_bit + 1)) - 1);
};

/* ----------------------------------------------------------------------- */
/* CORE FUNCTIONS */
/* ----------------------------------------------------------------------- */

uintptr_t get_or_create_l2_table(uintptr_t L1_bits)
{
    Table_Descriptor*       current_L1_table = &L1_table[L1_bits];

    if (current_L1_table->is_valid()) {
        return current_L1_table->get_next_level();
    };

    // CORRECT L1 Table DOESNOT EXIST

    uintptr_t current_l2_table_ptr = (uintptr_t)alloc_page();
    if (!current_l2_table_ptr) {
        ERR("get_or_create_l2_table: current_l2_table_ptr NULL");
        return ERR_ALLOC_FAILED;
    };

    current_L1_table->init();
    current_L1_table->set_next_table(current_l2_table_ptr);

    current_L1_table->validate();       /* COMMIT */

    /* ---------------------------------------------------------------------------------- */

    return current_L1_table->get_next_level();
};

uintptr_t get_or_create_l3_table(uintptr_t L2_table_ptr, uintptr_t va)
{
    size_t                  L2_index            = get_bits_from_ptr(va, 29, 21);
    Table_Descriptor*       current_L2_table    = &((Table_Descriptor*)L2_table_ptr)[L2_index];

    if (current_L2_table->is_valid()) {
        return current_L2_table->get_next_level();
    };

    // L2 Table DOESNOT EXIST

    uintptr_t L3_leaf = (uintptr_t)alloc_page();
    if (!L3_leaf) {
        ERR("get_or_create_l3_table: L3_leaf NULL");
        return ERR_ALLOC_FAILED;
    };

    current_L2_table->init();
    current_L2_table->set_next_table(L3_leaf);
    
    current_L2_table->validate();       /* COMMIT */

    return current_L2_table->get_next_level();
};

int setup_l3(uintptr_t L3_leaf, uintptr_t phy_addr, uintptr_t va)
{
    size_t              L3_index            = get_bits_from_ptr(va, 20, 12);
    Page_Descriptor*    current_L3_table    = &((Page_Descriptor *)L3_leaf)[L3_index];

    if (current_L3_table->is_valid()) {
        INFO("L3 Exists\n");
        return ERROR_ALREADY_MAPPED;
    };

    current_L3_table->init();
    current_L3_table->set_page_addr(phy_addr);
    current_L3_table->validate();               /* COMMIT */

    return SUCCESS;
};

int setup_table_4k(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions)
{
    pr_newline();

    INFO("setup_table_4k: phy: ");print_hex(phy);pr_newline();
    INFO("setup_table_4k: virt: ");print_hex(virt);pr_newline();

    /* Get L1 Table */
    uintptr_t L1_bits = get_bits_from_ptr(virt, 38, 30);

    INFO("setup_table_4k: run get_or_create_l2_table\n");

    auto L2_table = get_or_create_l2_table(L1_bits);
    if (L2_table == ERR_ALLOC_FAILED || L2_table == ERROR_NO_MEMORY || L2_table == ERR_UNKNOWN) {
        ERR("setup_table_4k: failed with error: ");
        print(L2_table);
        pr_newline();
        return L2_table;
    };

    /* ---------------------------------------------------------------------------------- */

    INFO("setup_table_4k: run get_or_create_l3_table\n");

    auto L3_leaf = get_or_create_l3_table(L2_table, virt);
    if (L3_leaf == ERR_ALLOC_FAILED || L3_leaf == ERROR_NO_MEMORY || L3_leaf == ERR_UNKNOWN || !L3_leaf) {
        ERR("setup_table_4k: get_or_create_l3_table wailed, err: ");
        print(L3_leaf);
        pr_newline();
        return L3_leaf;
    };

    /* ---------------------------------------------------------------------------------- */

    INFO("setup_table_4k: run setup_l3\n");

    int err = setup_l3(L3_leaf, phy, virt);
    if (err) {
        ERR("setup_table_4k: Error_In_L3: ");
        print(err);
        pr_newline();
        return err;
    };
    
    return 0;
};

void setup_tables()
{
    INFO("setup_tables: \n");
    uintptr_t base = (uintptr_t)getMemMap().memory_map->PhysicalStart;
    uintptr_t end  = base + getMemMap().memory_map->NumberOfPages*CSL_PAGE_SIZE;

    for (uintptr_t i = base; i < end; i += CSL_PAGE_SIZE) {
        int err = setup_table_4k(i, i, NONE);
        if (err) {
            ERR("setup_tables: Failed setup_table_4k with err: ");
            print(err);
            pr_newline();
            return;
        };
    };
};
