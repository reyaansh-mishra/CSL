/* src/mmu/orchestrator.c */

#include <utils.h>
#include <mmu/orchestrator_required.h>
#include <payload-includes/payload.h>
#include <bootstrappr.h>
#include <csl.h>
#include <mmu.h>
#include <terminal.h>
#include <arm64.h>
#include <memory.h>

#define UART0_BASE 0x09000000

struct MemMapprInfo memory_map;

static void snapshot_mem_map() {
    memory_map = getMemMap();
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <mmu orchestrator>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu orchestrator>: " fmt, ##__VA_ARGS__)

void map_csl_addrs(uintptr_t to) {
    size_t      itr     = 0;
    uintptr_t   base    = efi.csl_base;
    uintptr_t   end     = efi.csl_base + efi.csl_size;

    while ((base + itr*CSL_PAGE_SIZE) < end) {   // Do whatever we want here
        setup_table_for_page(
            base    + itr*CSL_PAGE_SIZE,
                to      + itr*CSL_PAGE_SIZE,
            (enum VIRT_ADDR_PERMISSIONS)(EXECUTABLE | WRITABLE)
        );
        itr++;
    };
};

void map_payload_addrs() {
    for (size_t i = 0; i < remap_addrs_count; i++) {
        struct PAYLOAD_REMAP_ADDRS* remap   = &remap_addrs[i];
        size_t                      itr     = remap->size/CSL_PAGE_SIZE;    // ASSUME Page-Multiple        

        for (size_t i = 0; i < itr; i++) {
            setup_table_for_page(
                remap->phy_start_addr   + i*CSL_PAGE_SIZE,
                remap->virt_start_addr      + i*CSL_PAGE_SIZE,
                remap->virtual_addr_permissions
            );
        };
    };
};

/*
 * 1. Snapshot Memory Map
 * 2. Start Mapping CSL
 * 3. Map Payload
 * 4. Identity Map UEFI [NOT DONE NOW]
 */

void setup_mmu_tables() {
    snapshot_mem_map();
    // uintptr_t csl_virtual_addr = payload_virtual_entry;
    
    INFO("IDENTITY MAPPING CSL!\n");
    map_csl_addrs(efi.csl_base);

    // ------------------------------------------------------------------------------------------------------------

    INFO("IDENTITY MAPPING PAYLOAD ADDRs!\n");
    map_payload_addrs();
};

/*
 * 1. Map the Addresses
 */

void setup_csl_for_jump() {
    uintptr_t   csl_jump_addr   = payload_reloc_physically;
    size_t      csl_size        = efi.csl_size;

    for (size_t i = 0; i < csl_size; i+=CSL_PAGE_SIZE) {
        setup_table_for_page(
                csl_jump_addr + i,
                    payload_virtual_entry + i,
            (enum VIRT_ADDR_PERMISSIONS)(EXECUTABLE | WRITABLE)
        );
    };
};

void jump() {
    if (payload_reloc_physically != efi.csl_base)
        move_csl_to_addr(payload_reloc_physically);
    else INFO("Did not jump.\n");
};

/*
 * 1. MASK INTERRUPTS
 * 2. setup_mmu_tables
 * 3. Configure CSL Payload Jump
 * 4. Enable MMU
 * 5. JUMP
 */

void start_mmu_work() {
    setup_mmu_tables();

    INFO("MAPPING COMPLETE!\n");
    if (payload_reloc_physically != efi.csl_base) {
        INFO("Prepping for Jump!\n");
        setup_csl_for_jump();
        INFO("Prepped for Jump!\n");
    };
    setup_table_for_page(UART0_BASE, UART0_BASE, WRITABLE);
    uintptr_t current_map = round_down(get_current_pc(), CSL_PAGE_SIZE);
    setup_table_for_page(current_map, current_map, EXECUTABLE);

    enable_malloc();

    efi.BootServices->ExitBootServices(efi.ImageHandle, getMemMap().map_key);
    INFO("Exited EFI Boot Services!\n");
    
    INFO("ENABLING MMU!\n");
    mmu_bs();
    jump();
};
