/* src/boot/bootstrappr.cpp */

#include <utils.h>

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
};

/**
 * Use: Start bootstrapping Payload's requirements now that CSL is alive
 */

void bootstrappr(struct MemMapprInfo mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */
    size_t itr              = 0;
    uint8_t*    entry       = (uint8_t*)mem_info.memory_map;
    uint8_t*    end         = entry + mem_info.memory_map_size; // memory_map_size should be total bytes here

    while (entry < end) {
        entry += mem_info.descriptor_size;
        itr++;
    };

    INFO("Entries: %lu\n", itr);
    uint64_t current_pc = 0;

    __asm__ volatile(
        "mrs %0, elr_el2"
        : "=r"(current_pc)
        :
        :
    );

    INFO("Current VA PC = %lx\n", current_pc);

    INFO("RUN MMU\n");
    setup_tables();
};

[[noreturn]] void csl_continue_if_needed()
{
    INFO("Setting Up boot_info...\n");
    setup_bootinfo();
    
    payload_main(boot_info);

    ERR("PAYLOAD RETURNED! BUSY LOOPING!\n");
        while (true) {
        __asm__ volatile("wfi");
    };
};

