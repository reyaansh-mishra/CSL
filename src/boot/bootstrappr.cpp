#include <utils.hpp>
#include <payload-includes/payload.h>

struct FreeRegion FreeRegion[MAX_FREE_REGIONS];

void bootstrappr(struct MemMapprInfo mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */

    INFO("sizeof(EFI_MEMORY_DESCRIPTOR): ");
    print(sizeof(EFI_MEMORY_DESCRIPTOR));
    pr_newline();

    INFO("descriptor_size: ");
    print((uint64_t)mem_info.descriptor_size);
    pr_newline();

    INFO("bootstrappr: Printing ALL entries of mem_info...\n");
    size_t itr = 0;
    uint8_t*    entry       = (uint8_t*)mem_info.memory_map;
    uint8_t*    end         = entry + mem_info.memory_map_size; // memory_map_size should be total bytes here
    size_t      type_7_ctr  = 0;

    while (entry < end) {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)entry;

        uint64_t phy_start = desc->PhysicalStart;
        uint64_t no_of_pgs = desc->NumberOfPages;
        uint64_t uefi_type = desc->Type;

        print(phy_start);
        pr_newline();

        print("    ");
        print(no_of_pgs);
        pr_newline();

        print("    ");
        print(uefi_type);
        pr_newline();
        pr_newline();

        if (uefi_type == 7) {
            FreeRegion[type_7_ctr].base         = phy_start;
            FreeRegion[type_7_ctr].page_count   = no_of_pgs;

            type_7_ctr++;
        };

        entry += mem_info.descriptor_size;
        itr++;
    }
    

    INFO("Entries: ");
    print(itr);
    pr_newline();

    INFO("Type 7 Entries: ");
    print(type_7_ctr);
    pr_newline();


    payload_main();
    return;
};