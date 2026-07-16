#include "Base.h"
#include <csl.h>
#include <utils.h>

#include <Uefi.h>

EFI_CONTEXT efi_context;

static void csl_main(void) {
    print("CSL Version ");
    print(CSL_VERSION);
    print("\n\r");
    print("Hi Guys!\n");
};


EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    efi_context.this = ImageHandle;
    efi_context.SystemTable = SystemTable;

    csl_main();

    while (TRUE) {
        __asm__ volatile("wfi");
    };

    return EFI_SUCCESS;
};
