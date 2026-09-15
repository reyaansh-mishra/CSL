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
    boot_info.ImageBase     = efi.csl_base;
    boot_info.ImageSize     = efi.csl_size;
    
    boot_info.ram_base      = efi.ram_base;
    boot_info.ram_size      = efi.ram_size;
};

/*
 * Start bootstrapping Payload's requirements now that CSL is alive
 */

void bootstrappr(struct MemMapprInfo mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */
    size_t      itr __attribute__((unused));
    itr = 0;
    uint8_t*    entry       = (uint8_t*)mem_info.memory_map;
    uint8_t*    end         = entry + mem_info.memory_map_size; // memory_map_size should be total bytes here
    bool        we_know_ram_base = false;

    uint64_t ram_bytes = 0;

    /* AI GENERATED */
    while (entry < end) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)entry;

        if (desc->Type == EfiConventionalMemory) {
            if (!we_know_ram_base) {
                efi.ram_base = desc->PhysicalStart;
                we_know_ram_base = true;
            }
            ram_bytes += desc->NumberOfPages * CSL_PAGE_SIZE;
        }

        entry += mem_info.descriptor_size;
        itr++;
    }
    efi.ram_size = ram_bytes;
    /* END OF AI GENERATED */

    // INFO("Entries: %lu\n", itr);
    // uintptr_t current_pc = get_current_pc();
    // INFO("Current VA PC = %p\n", current_pc);

    // bool debug_waiting = 1;
    // INFO("Text At: %p\n", efi.csl_base + 0x1000);
    // INFO("Waiting for Debugger to set debug_waiting == 0....\n");
    // while (debug_waiting) {
    //     __asm__ volatile("yield");
    // }

    if (payload_reloc_physically == 0) {
        if (payload_virtual_entry == 0) {
            payload_virtual_entry = efi.csl_base;
        }
        payload_reloc_physically = efi.csl_base;
    } else if (payload_virtual_entry == 0) {
        payload_virtual_entry = efi.csl_base;
    };

    INFO("SETUP STACK && START MMU WORK\n");
    // INFO("start_mmu_work addr: %p\n", start_mmu_work);
    setup_stack((uintptr_t)start_mmu_work);
};

void csl_continue_if_needed()
{
    install_vbar(); // Reinstall because VBARS have changed
    INFO("Setting Up boot_info...\n");
    setup_bootinfo();
    
    payload_main(boot_info);

    ERR("PAYLOAD RETURNED! EXITING!\n");
    SYSTEM_POWEROFF();
};

