/* src/boot/bootstrappr.cpp */

#include <utils.h>
#include <mmu.h>
#include <arm64.h>
#include <stack.h>

#include <specific-includes/bootstrappr.h>
#include <specific-includes/terminal.h>

#include <payload-includes/payload.h>


#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[CSL] <bootstrappr>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)  print("[ERR] [CSL] <bootstrappr>: " fmt, ##__VA_ARGS__)


void setup_tables();
struct PAYLOAD_BOOT_INFO boot_info;

static void setup_bootinfo() {
    boot_info.ImageBase_phy     = efi.csl_base_phy;
    boot_info.ImageBase_virt    = efi.csl_base_virt;
    boot_info.ImageSize         = efi.csl_size;    
};

/*
 * Start bootstrapping Payload's requirements now that CSL is alive
 */

void bootstrappr(UEFI_MEMORY_MAP mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */
    size_t      itr __attribute__((unused));
    itr = 0;
    uint8_t*    entry       = (uint8_t*)mem_info.memory_map;
    uint8_t*    end         = entry + mem_info.memory_map_size; // memory_map_size should be total bytes here

    while (entry < end) {
        entry += mem_info.descriptor_size;
        itr++;
    }

    // bool debug_waiting = 1;
    // INFO("Text At: %p\n", efi.csl_base_phy + 0x1000);
    // INFO("Waiting for Debugger to set debug_waiting == 0....\n");
    // while (debug_waiting) {
    //     __asm__ volatile("yield");
    // }

    boot_info.uefi_memory_map = mem_info;

    if (!payload_reloc_physically)  {
        payload_reloc_physically = efi.csl_base_phy;
        INFO("Payload Phy Reloc was ZERO\n");
    }
    if (!payload_virtual_entry) {
        payload_virtual_entry = efi.csl_base_phy;
        INFO("Payload Virt Reloc was ZERO\n");
    }

    INFO("SETUP STACK && START MMU WORK\n");
    // add_virtual_mapping(    // Ident Map the memory map itself, because we need it
    //     (uintptr_t)mem_info.memory_map,
    //     (uintptr_t)mem_info.memory_map,
    //     mem_info.memory_map_size,
    //     READ_ONLY
    // );
    setup_stack((uintptr_t)start_mmu_work);
};

[[noreturn]] void csl_continue_if_needed()
{
    install_vbar(); // Reinstall because VBARS have changed
    INFO("Setting Up boot_info...\n");
    setup_bootinfo();
    
    payload_main(boot_info);

    ERR("PAYLOAD RETURNED! EXITING!\n");
    SYSTEM_POWEROFF();
    __builtin_unreachable();
};

