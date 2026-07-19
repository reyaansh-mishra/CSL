#include <utils.hpp>
#include <payload-includes/payload.h>

void bootstrappr(struct MemMapprInfo mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */

    INFO("sizeof(EFI_MEMORY_DESCRIPTOR): ");
    print(sizeof(EFI_MEMORY_DESCRIPTOR));
    pr_newline();

    INFO("descriptor_size: ");
    print((uint64_t)mem_info.descriptor_size);
    pr_newline();

    INFO("bootstrappr: Printing ALL entries of mem_info...\n");

    payload_main();
    return;
};