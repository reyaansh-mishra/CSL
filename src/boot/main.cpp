/* 
 * src/boot/main.cpp 
 * ENTRY FILE 
 */

#include <utils.hpp>
#include <specific-includes/block_allocator.hpp>

extern "C" {
    #include <csl.h>
    #include <specific-includes/terminal.h>
    #include <specific-includes/bootstrappr.h>
    #include <specific-includes/arm64.h>
    #include <payload-includes/payload.h>
    #include <memory.h>
};

#include <Protocol/LoadedImage.h>


#ifdef CSL_FAKE_PAYLOAD_TEST
EFI_STATUS EFIAPI payload_init();
#endif

// Loaded Image Protocol GUID
EFI_GUID gEfiLoadedImageProtocolGuid = 
    { 0x5B1B31A1, 0x9562, 0x11D2, { 0x8E, 0x3F, 0x00, 0xA0, 0xC9, 0x69, 0x72, 0x3B } };

// Simple Text Output Protocol GUID
EFI_GUID gEfiSimpleTextOutProtocolGuid = 
    { 0xD35EE3B1, 0x5775, 0x11D1, { 0x9A, 0x60, 0x00, 0x80, 0xC7, 0x3C, 0x37, 0x19 } };

bool            pls_use_malloc_now;
uintptr_t       payload_virtual_entry;
uint64_t        payload_reloc_physically;


EFI_CONTEXT efi;
bool drop_to_el1 = false;

static EFI_STATUS EFIAPI csl_main(void)
{ /* Actually run CSL */

    payload_reloc_physically = round_down(payload_reloc_physically, CSL_PAGE_SIZE);

    INFO("CSL v%s,\n\tRevision Desc: \"%s\"\n", CSL_VERSION, CSL_VERSION_DESC);
    INFO("BASE = %p, SIZE = %d\n", efi.csl_base_phy, efi.csl_size);

    INFO("!!! STARTING CORE CSL APPLICATION !!!\n");
    bootstrappr(getMemMap());
    return EFI_SUCCESS;
};

/**
 * Entry point.
 * Use:     Get a minimal CSL runtime up before Payload sets its configs up.
 */

extern "C" EFI_STATUS EFIAPI csl_bootstrap(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{  /* Setup Core CSL UEFI Runtime */
    mask_FULL();

    if (get_current_el() != 2) {
        not_in_el2();
    };

    pls_use_malloc_now          = false;

    efi.ImageHandle             = ImageHandle;
    efi.SystemTable             = SystemTable;
    efi.BootServices            = efi.SystemTable->BootServices;
    payload_virtual_entry       = 0;
    payload_reloc_physically    = 0;

    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;

    EFI_STATUS status = efi.BootServices->OpenProtocol(efi.ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&LoadedImage, ImageHandle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL);
    if (EFI_ERROR(status)) { return status; };

    efi.csl_base_phy    = (uintptr_t)LoadedImage->ImageBase;
    efi.csl_size        = round_up(LoadedImage->ImageSize, CSL_PAGE_SIZE);

    terminal_reset();
    int err = payload_init();

    if (err != EFI_SUCCESS) {
        ERR("CSL_BOOT_STUB: Unable to conitnue, Err: %lu\n", err);
        return err;
    };
    SYSTEM_HALT();
    return EFI_SUCCESS;
};

/* Current CSL TEST Builds */

#ifdef CSL_FAKE_PAYLOAD_TEST

#include <payload-includes/payload.h>
#undef  INFO
#define INFO(string) print("[PAYLOAD]: %s", string)

EFI_STATUS EFIAPI payload_init()
{
    // add_virtual_mapping(0x0000, 0xFF000, CSL_PAGE_SIZE*100, READ_ONLY);
    payload_reloc_physically    = 0x40000000;
    payload_virtual_entry       = 0x100000000;
    return csl_main();
};

extern "C" void payload_main(struct PAYLOAD_BOOT_INFO boot_struct) {
    INFO("PAYLOAD START\n");

    INFO("PRINTING BOOT INFO STRUCT:\n");
    print("\t\tImageBase_phy = %p, ImageSize = %lu, BootArgs (NOT SUPPORTED YET) = %c\n", boot_struct.ImageBase_phy, boot_struct.ImageSize, boot_struct.BootArgs);
};

#endif
