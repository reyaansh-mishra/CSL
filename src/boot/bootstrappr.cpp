/* src/boot/bootstrappr.cpp */

#include <utils.hpp>
#include <payload-includes/payload.h>

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

    if (payload_virtual_entry != 0) {
        add_virtual_mapping(efi.csl_base, payload_virtual_entry, efi.csl_size, EXECUTABLE);
    };

    INFO("RUN MMU\n");
    setup_tables();
    
    payload_main();
    return;
};
