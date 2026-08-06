/* src/mmu/translation_tables.cpp */

#include <utils.hpp>
#include <payload-includes/payload.h>

#define TTBR_BASE               (uint64_t)&L1_table[0]
#define MALLOC_MAX_BLOCK_PAGES  100
// Common ARM64 MAIR attributes:
#define MAIR_ATTR_DEVICE_nGnRnE  0x00  // Attr0: Device memory (UART, GIC, MMIO)
#define MAIR_ATTR_NORMAL_NOCACHE 0x44  // Attr1: Normal Uncached RAM
#define MAIR_ATTR_NORMAL_WB      0xFF  // Attr2: Normal Write-Back Cacheable RAM

/* ----------------------------------------------------------------------- */
/* GLOBALS */
/* ----------------------------------------------------------------------- */

alignas(4096) Table_Descriptor      L1_table[512];
size_t                              L3_Leaf_entries;

/* ----------------------------------------------------------------------- */
/* HELPERS */
/* ----------------------------------------------------------------------- */

static inline uintptr_t get_bits_from_ptr(uintptr_t ptr, unsigned start_bit, unsigned stop_bit)   /* from Start -> Stop */
{
    return (ptr >> stop_bit) & ((1ULL << (start_bit - stop_bit + 1)) - 1);
};

static void ensure_malloc_exists()
{
    void* alloc_addr = alloc_pages(MALLOC_MAX_BLOCK_PAGES, EfiLoaderData);
    allocator.init(alloc_addr, MALLOC_MAX_BLOCK_PAGES);

    pls_use_malloc_now = true;
};

static void configure_mmu()
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

    INFO("Applied Private Translation Tables!\n");
};

/* ----------------------------------------------------------------------- */
/* CORE FUNCTIONS */
/* ----------------------------------------------------------------------- */

static uintptr_t get_or_create_l2_table(uintptr_t L1_bits)
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

static uintptr_t get_or_create_l3_table(uintptr_t L2_table_ptr, uintptr_t va)
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

static struct L3_Page_Descriptor_Info set_l3_perms(enum VIRT_ADDR_PERMISSIONS permissions)
{
    help_me_build_page_entry table;

    table.set_default_values();

    if ((permissions & READ_ONLY) != 0) {
        table.set_exec(EXEC_UNAVAIL);
    };

    if ((permissions & EXECUTABLE) != 0) {
        table.set_exec(EXEC_AVAIL);
    } else {
        table.set_exec(EXEC_UNAVAIL);
    };

    if ((permissions & WRITABLE) != 0) {
        table.set_rw_perms(EL2_RW);
    } else {
        table.set_rw_perms(EL2_RO);
    };
    return table.get();
};

static int setup_l3(uintptr_t L3_leaf, uintptr_t phy_addr, uintptr_t va, enum VIRT_ADDR_PERMISSIONS permissions)
{
    size_t              L3_index            = get_bits_from_ptr(va, 20, 12);
    Page_Descriptor*    current_L3_table    = &((Page_Descriptor *)L3_leaf)[L3_index];

    if (current_L3_table->is_valid()) {
        INFO("L3 Exists, phy_addr = %lx, to = %lx\n", phy_addr, current_L3_table->get_page_addr());
        return SUCCESS;
    };

    auto table = set_l3_perms(permissions);

    current_L3_table->init();
    current_L3_table->validate(table, phy_addr);    /* COMMIT */
    L3_Leaf_entries++;

    return SUCCESS;
};

static int setup_table_4k(uintptr_t phy, uintptr_t virt, enum VIRT_ADDR_PERMISSIONS permissions)
{

    ASSERT((phy & 0xFFF) == 0);
    ASSERT((virt & 0xFFF) == 0);

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

    int err = setup_l3(L3_leaf, phy, virt, permissions);
    if (err) {
        ERR("setup_table_4k: Error_In_L3: ");
        print(err);
        pr_newline();
        return err;
    };
    
    return 0;
};

static void identity_map_all(struct MemMapprInfo map_info, uint8_t* map_buf, size_t num_entries)
{
    INFO("Identity Mapping CSL!\n");
    for (uintptr_t cursor_4k = efi.csl_base; cursor_4k < efi.csl_base + efi.csl_size; cursor_4k += CSL_PAGE_SIZE) {
        int err = setup_table_4k(cursor_4k, cursor_4k, EXECUTABLE);
        if (err) {
            ERR("ERROR: setup_table_4k, %lu", err);
            return;
        };
    };

    INFO("Identity Mapping MMIO!\n");
    for (uintptr_t mmio = 0x08000000; mmio < 0x0A000000; mmio += CSL_PAGE_SIZE) {
        int err = setup_table_4k(mmio, mmio, WRITABLE);
        if (err) {
            ERR("ERROR: setup_table_4k, %lu", err);
            return;
        };
    };

    INFO("Identity Mapping Rest of UEFI RAM!\n");
    for (size_t i = 0; i < num_entries; i++) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)(map_buf + (i * map_info.descriptor_size));

        uintptr_t base = desc->PhysicalStart;
        uintptr_t size = desc->NumberOfPages * CSL_PAGE_SIZE;

        // Identity map every physical memory descriptor provided by UEFI
        for (uintptr_t addr = base; addr < base + size; addr += CSL_PAGE_SIZE) {
            int err = setup_table_4k(addr, addr, (enum VIRT_ADDR_PERMISSIONS)(EXECUTABLE | WRITABLE));
            if (err) {
                ERR("ERROR: setup_table_4k, %lu", err);
                return;
            };
        };
    };
};

static void identity_map_payload()
{
    INFO("Identity Mapping Payload\n");
    for (size_t i = 0; i < PAYLOAD_MAX_REMAP_ADDRS; i++) {
        if (remap_addrs[i].active) {
            for (size_t size_4096 = 0; size_4096 < remap_addrs[i].size; size_4096 += 4096) {
                INFO("For Phy Addr: %lx, Virt Addr = %lx, with Size = %lu\n", remap_addrs[i].phy_start_addr+size_4096, remap_addrs[i].virt_start_addr+size_4096, size_4096);
                int err = setup_table_4k(remap_addrs[i].phy_start_addr + size_4096, remap_addrs[i].virt_start_addr + size_4096, remap_addrs[i].virtual_addr_permissions);
                if (err) {
                    ERR("ERROR: setup_table_4k, %lu", err);
                    return;
                };
            };
        }
    };
};

void setup_tables() {
    auto map_info = getMemMap();
    uint8_t* map_buf = (uint8_t*)map_info.memory_map;
    size_t num_entries = map_info.memory_map_size / map_info.descriptor_size;

    identity_map_payload();
    identity_map_all(map_info, map_buf, num_entries);

    INFO("IDENTITY MAPPING COMPLETE!\n");

    configure_mmu();
    unmask_interrupts();
    // INFO("L3 Descriptors Used: %d, Current MAX: %lu\n", L3_Leaf_entries, MAX_L3_DESCRIPTORS);

    // volatile uint64_t *ptr = (uint64_t *)0x40000000;
    // uint64_t x = *ptr; // should work
    // *ptr = 1234;       // should fault
};
