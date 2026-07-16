#include <csl.h>
#include <utils.h>

#include <Uefi.h>

EFI_HANDLE EFI_ImageHandle;
EFI_SYSTEM_TABLE *EFI_SystemTable;

static void csl_main(void) {
    print("CSL Version ");
    print(CSL_VERSION);
    print("\n");
    print("Hi Guys!\n");
};


EFI_STATUS EFIAPI efi_main(
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
)
{
    SystemTable->ConOut->OutputString(
        SystemTable->ConOut,
        L"CSL Version 0.001\r\n"
    );

    return EFI_SUCCESS;
}