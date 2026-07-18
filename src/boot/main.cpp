#include "specific-includes/memory.hpp"
#include <csl.h>
#include <stdint.h>
#include <utils.hpp>

#include <Uefi.h>

EFI_CONTEXT efi;

static int csl_main(void)
{ /* Actually run CSL */

    uint32_t currentEL = get_current_el();

    INFO("CSL Version ");
    print(CSL_VERSION);
    print("\n");
 
    if (currentEL != 2) {
        not_in_el2();
    };

    mem_map_init();

    struct MemMapprInfo mem_map = getMemMap();

    // INFO(is1GbAligned(mem_map.memory_map->PhysicalStart));
    bootstrappr(mem_map);

    return 0;
};

extern "C" EFI_STATUS EFIAPI csl_init(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{  /* Setup Core CSL UEFI Runtime */
    efi.ImageHandle     = ImageHandle;
    efi.SystemTable     = SystemTable;
    efi.BootServices    = efi.SystemTable->BootServices;

    terminal_reset();
    int err = csl_main();

    if (err != 0) {
        ERR("CSL_BOOT_STUB: Unable to conitnue, Err: ");
        print(err);
        print("\n");

        return EFI_ERROR(err);
    };

    while (TRUE) {
        __asm__ volatile("wfi");
    };

    return EFI_SUCCESS;
};

/* Current CSL TEST Builds */

#ifdef CSL_FAKE_PAYLOAD_TEST
extern "C" EFI_STATUS EFIAPI payload_init(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    return csl_init(ImageHandle, SystemTable);
};

void payload_main() {
    INFO("PAYLOAD START\n");
};

#endif