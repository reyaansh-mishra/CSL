#include <csl.h>
#include <utils.hpp>

#ifdef CSL_FAKE_PAYLOAD_TEST
EFI_STATUS EFIAPI payload_init();
#endif

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

extern "C" EFI_STATUS EFIAPI csl_bootstrap(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{  /* Setup Core CSL UEFI Runtime */
    efi.ImageHandle     = ImageHandle;
    efi.SystemTable     = SystemTable;
    efi.BootServices    = efi.SystemTable->BootServices;

    terminal_reset();
    int err = payload_init();

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

#include <payload-includes/payload.h>
#undef INFO
#define INFO(string) pr_info("[PAYLOAD]: ", string)

EFI_STATUS EFIAPI payload_init()
{
    add_virtual_mapping(2,4,2,NONE);
    return csl_main();
};

void payload_main() {
    INFO("PAYLOAD START\n");
};

#endif