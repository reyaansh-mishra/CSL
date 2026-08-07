/* src/boot/bootstrappr.cpp */

#include <utils.hpp>
#include <payload-includes/payload.h>

struct PAYLOAD_BOOT_INFO boot_info;

static void setup_bootinfo() {
    boot_info.ImageBase     = efi.csl_base;
    boot_info.ImageSize     = efi.csl_size;
};

void bootstrappr(struct MemMapprInfo mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */

    #undef INFO
    #undef ERR
    #define INFO(string)    print("[CSL] <bootstrappr>: %s", (string))
    #define ERR(string)     print("[ERR] [CSL] <bootstrappr>: %s", (string))

    size_t itr              = 0;
    uint8_t*    entry       = (uint8_t*)mem_info.memory_map;
    uint8_t*    end         = entry + mem_info.memory_map_size; // memory_map_size should be total bytes here

    while (entry < end) {
        entry += mem_info.descriptor_size;
        itr++;
    };

    INFO("Entries: ");
    print(itr);
    pr_newline();

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

