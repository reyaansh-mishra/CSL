/* src/mmu/mmu.c */
#include <utils.hpp>

#define TTBR_BASE               (uint64_t)&L1_table[0]
#define MALLOC_MAX_BLOCK_PAGES  100
// Common ARM64 MAIR attributes:
#define MAIR_ATTR_DEVICE_nGnRnE  0x00  // Attr0: Device memory (UART, GIC, MMIO)
#define MAIR_ATTR_NORMAL_NOCACHE 0x44  // Attr1: Normal Uncached RAM
#define MAIR_ATTR_NORMAL_WB      0xFF  // Attr2: Normal Write-Back Cacheable RAM

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
        return SUCCESS;
    };

    current_L3_table->init();
    current_L3_table->set_page_addr(phy_addr);
    current_L3_table->validate();               /* COMMIT */

    return SUCCESS;
};

int setup_table_4k(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions)
{
    // pr_newline();

    // INFO("setup_table_4k: phy: ");print_hex(phy);pr_newline();
    // INFO("setup_table_4k: virt: ");print_hex(virt);pr_newline();

    /* Get L1 Table */
    uintptr_t L1_bits = get_bits_from_ptr(virt, 38, 30);

    // INFO("setup_table_4k: run get_or_create_l2_table\n");

    auto L2_table = get_or_create_l2_table(L1_bits);
    if (L2_table == ERR_ALLOC_FAILED || L2_table == ERROR_NO_MEMORY || L2_table == ERR_UNKNOWN) {
        ERR("setup_table_4k: failed with error: ");
        print(L2_table);
        pr_newline();
        return L2_table;
    };

    /* ---------------------------------------------------------------------------------- */

    // INFO("setup_table_4k: run get_or_create_l3_table\n");

    auto L3_leaf = get_or_create_l3_table(L2_table, virt);
    if (L3_leaf == ERR_ALLOC_FAILED || L3_leaf == ERROR_NO_MEMORY || L3_leaf == ERR_UNKNOWN || !L3_leaf) {
        ERR("setup_table_4k: get_or_create_l3_table wailed, err: ");
        print(L3_leaf);
        pr_newline();
        return L3_leaf;
    };

    /* ---------------------------------------------------------------------------------- */

    // INFO("setup_table_4k: run setup_l3\n");

    int err = setup_l3(L3_leaf, phy, virt);
    if (err) {
        ERR("setup_table_4k: Error_In_L3: ");
        print(err);
        pr_newline();
        return err;
    };
    
    return 0;
};

void ensure_malloc_exists()
{
    void* alloc_addr = alloc_pages(MALLOC_MAX_BLOCK_PAGES, EfiLoaderData);
    allocator.init(alloc_addr, MALLOC_MAX_BLOCK_PAGES);
};

void configure_mmu()
{

    ensure_malloc_exists();
    

    INFO("EXITING BOOT SERVICES!\n");
    efi.BootServices->ExitBootServices(efi.ImageHandle, getMemMap().map_key);

    if (get_current_el() != 2) {
        ERR("EL NOT AT 2! EXITING!\n");
        return;
    };

    mask_FULL();
    install_vbar();

    INFO("Interrupts Masked and VBARS installed.\n");


    uint64_t tcr    = 0;
    uint64_t T0SZ   = 25;
    uint64_t TG0    = 0b00;
    uint64_t PS     = 0b0010;
    uint64_t ATTR   = 0b0101;
    uint64_t SH0    = 0b11;
    for (size_t i = 0; i < 6; i++) {
        set_bit(tcr, i, get_bit(T0SZ, i));
    };
    for (size_t i = 8; i < 12; i++) {
        set_bit(tcr, i, get_bit(ATTR, i - 8));
    };
    for (size_t i = 12; i < 14; i++) {
        set_bit(tcr, i, get_bit(SH0, i - 12));
    };
    for (size_t i = 14; i < 16; i++) {
        set_bit(tcr, i, get_bit(TG0, i - 14));
    };
    for (size_t i = 16; i < 20; i++) {
        set_bit(tcr, i, get_bit(PS, i - 16));
    };

    uint64_t mair_val = 
        ((uint64_t)MAIR_ATTR_DEVICE_nGnRnE  << (0 * 8)) |  // Index 0
        ((uint64_t)MAIR_ATTR_NORMAL_NOCACHE << (1 * 8)) |  // Index 1
        ((uint64_t)MAIR_ATTR_NORMAL_WB      << (2 * 8));   // Index 2

    disable_mmu();
    write_mair(mair_val);
    write_tcr(tcr);
    write_ttbr0(TTBR_BASE);
    enable_mmu();

    uint64_t SCTLR_EL2_VAL;
    __asm__ volatile(
        "msr SCTLR_EL2, x0"
        : "=r" (SCTLR_EL2_VAL)
        :
        :
    );

    INFO("Applied Private Translation Tables!\n");

    auto a1 = allocator.malloc(2);
    auto a2 = allocator.malloc(5);

    print_hex((uint64_t)a1);pr_newline();
    print_hex((uint64_t)a2);pr_newline();

    INFO("malloc'd\n");

    allocator.dealloc(a1);
    allocator.dealloc(a2);

    INFO("DEALLOC'd\n");

    print_hex((uint64_t)allocator.malloc(12));
    INFO("Alloc'd Again\n");
};


void setup_tables() {
    auto map_info = getMemMap();
    uint8_t* map_buf = (uint8_t*)map_info.memory_map;
    size_t num_entries = map_info.memory_map_size / map_info.descriptor_size;

    for (size_t i = 0; i < num_entries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)(map_buf + (i * map_info.descriptor_size));

        uintptr_t base = desc->PhysicalStart;
        uintptr_t size = desc->NumberOfPages * CSL_PAGE_SIZE;

        // Identity map every physical memory descriptor provided by UEFI
        for (uintptr_t addr = base; addr < base + size; addr += CSL_PAGE_SIZE) {
            setup_table_4k(addr, addr, NONE);
        }
    }

    for (uintptr_t mmio = 0x08000000; mmio < 0x0A000000; mmio += CSL_PAGE_SIZE) {
        setup_table_4k(mmio, mmio, NONE);
    }
    configure_mmu();
};
