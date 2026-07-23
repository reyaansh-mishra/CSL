/* 
 * src/boot/main.cpp 
 * ENTRY FILE 
 */

#include <utils.hpp>

#ifdef CSL_FAKE_PAYLOAD_TEST
EFI_STATUS EFIAPI payload_init();
#endif

EFI_CONTEXT efi;

static EFI_STATUS EFIAPI csl_main(void)
{ /* Actually run CSL */

    INFO("CSL Version ");
    print(CSL_VERSION);
    print("\n");

    int err = mem_map_init();
    if (err != SUCCESS) {
        ERR("csl_main: FAILED mem_map_init WITH ERR: ");
        print(err);

        return EFI_DEVICE_ERROR;
    };

    struct MemMapprInfo mem_map = getMemMap();

    bootstrappr(mem_map);

    return EFI_SUCCESS;
};

extern "C" EFI_STATUS EFIAPI csl_bootstrap(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{  /* Setup Core CSL UEFI Runtime */

    #ifndef BUILD_FOR_AMD64
    if (get_current_el() != 2) {
        not_in_el2();
        install_vbar();
    };
    
    mask_interrupts();
    #endif

    efi.ImageHandle     = ImageHandle;
    efi.SystemTable     = SystemTable;
    efi.BootServices    = efi.SystemTable->BootServices;

    terminal_reset();
    int err = payload_init();

    if (err != EFI_SUCCESS) {
        ERR("CSL_BOOT_STUB: Unable to conitnue, Err: ");
        print(err);
        print("\n");

        return err;
    };

    #ifndef BUILD_FOR_AMD64
    while (TRUE) {
        __asm__ volatile("wfi");
    };
    #else
    while (TRUE) {
        __asm__ volatile("hlt");
    };
    #endif
    return EFI_SUCCESS;
};

/* Current CSL TEST Builds */

#ifdef CSL_FAKE_PAYLOAD_TEST

#include <payload-includes/payload.h>
#undef INFO
#define INFO(string) pr_info("[PAYLOAD]: ", string)

EFI_STATUS EFIAPI payload_init()
{
    add_virtual_mapping(2, 0xFF, CSL_PAGE_SIZE*100, NONE);

    return csl_main();
};

void payload_main() {
    INFO("PAYLOAD START\n");
};

#endif
