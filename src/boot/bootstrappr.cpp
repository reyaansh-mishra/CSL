/* src/boot/bootstrappr.cpp */

extern "C" {
    #include <terminal.h>
    #include <memory.h>
    #include <payload-includes/payload.h>
};

#include <mmu.hpp>
#include <utils.hpp>

struct PAYLOAD_BOOT_INFO boot_info;

static void setup_bootinfo() {
    boot_info.ImageBase     = efi.csl_base;
    boot_info.ImageSize     = efi.csl_size;
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)    print("[CSL] <bootstrappr>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)     print("[ERR] [CSL] <bootstrappr>: " fmt, ##__VA_ARGS__)


void bootstrappr(struct MemMapprInfo mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */

    size_t itr              = 0;
    uint8_t*    entry       = (uint8_t*)mem_info.memory_map;
    uint8_t*    end         = entry + mem_info.memory_map_size; // memory_map_size should be total bytes here

    while (entry < end) {
        entry += mem_info.descriptor_size;
        itr++;
    };

    INFO("Entries: %lu", itr);
    INFO("RUN MMU\n");
    setup_tables();
};

extern "C" [[noreturn]] void csl_continue_if_needed()
{
    INFO("Setting Up boot_info...\n");
    setup_bootinfo();
    
    payload_main(boot_info);

    ERR("PAYLOAD RETURNED! BUSY LOOPING!\n");
        while (true) {
        __asm__ volatile("wfi");
    };
};

