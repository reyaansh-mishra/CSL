#include "Base.h"
#include <csl.h>
#include <stdint.h>
#include <utils.hpp>

#include <Uefi.h>

EFI_CONTEXT efi_context;

static void csl_main(void) {

    uint32_t currentEL = get_current_el();

    print("CSL Version ");
    print(CSL_VERSION);
    print("\n");
    print("Hi Guys!\n");
    print("Current EL: ");
    print(currentEL);
    
};


extern "C" EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    efi_context.ImageHandle = ImageHandle;
    efi_context.SystemTable = SystemTable;

    csl_main();

    while (TRUE) {
        __asm__ volatile("wfi");
    };

    return EFI_SUCCESS;
};