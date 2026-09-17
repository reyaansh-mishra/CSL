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

UEFI_MEMORY_MAP memory_map;
uint64_t csl_base_old = 0;

static void snapshot_mem_map() {
    memory_map = getMemMap();
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <mmu orchestrator>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [CSL] <mmu orchestrator>: " fmt, ##__VA_ARGS__)

void map_csl_addrs(uintptr_t to) {
    size_t      itr     = 0;
    uintptr_t   base    = efi.csl_base_phy;
    uintptr_t   end     = efi.csl_base_phy + efi.csl_size;

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
    INFO("IDENTITY MAPPING CSL!\n");
    map_csl_addrs(efi.csl_base_phy);

    // ------------------------------------------------------------------------------------------------------------

    INFO("IDENTITY MAPPING PAYLOAD ADDRs!\n");
    map_payload_addrs();
};

/*
 * 1. Map the Addresses
 * 2. Build the Offset
 * 3. RETURN
 */

uintptr_t setup_csl_for_jump(uintptr_t addr) {
    size_t      csl_size        = efi.csl_size;

    for (size_t i = 0; i <= csl_size; i+=CSL_PAGE_SIZE) {
        setup_table_for_page(
                addr + i,
                    payload_virtual_entry + i,
            (enum VIRT_ADDR_PERMISSIONS)(EXECUTABLE | WRITABLE)
        );
    };

    uintptr_t offset = (uintptr_t)&csl_continue_if_needed - efi.csl_base_phy;
    csl_base_old = efi.csl_base_phy;
    efi.csl_base_phy = addr;
    memcpy((void*)addr, (void*)csl_base_old, efi.csl_size);
    return offset;
};

void jump(uintptr_t offset) {
    if (payload_virtual_entry != csl_base_old)
        move_csl_to_addr(payload_virtual_entry + offset);
    else {
        INFO("Did not jump.\n\tpayload_reloc = %p", payload_virtual_entry);
    }
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
    uintptr_t offset = 0;

    INFO("MAPPING COMPLETE!\n");
    if (payload_reloc_physically != efi.csl_base_phy) {
        INFO("Prepping for Jump!\n");
        offset = setup_csl_for_jump(payload_reloc_physically);
        INFO("Prepped for Jump!\n");
    }
    setup_table_for_page(UART0_BASE, UART0_BASE, WRITABLE);
    uintptr_t current_map = round_down(get_current_pc(), CSL_PAGE_SIZE);
    setup_table_for_page(current_map, current_map, EXECUTABLE);

    enable_malloc();

    efi.BootServices->ExitBootServices(efi.ImageHandle, getMemMap().map_key);
    INFO("Exited EFI Boot Services!\n");
    disable_mmu();
    
    INFO("ENABLING MMU!\n");
    mmu_bs();

    if (payload_reloc_physically != efi.csl_base_phy)
        jump(offset);
    else
        csl_continue_if_needed();
};
