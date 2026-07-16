#pragma once
#include <Uefi.h>

#define CSL_VERSION "0.0.1"


typedef struct {
    EFI_HANDLE          ImageHandle;
    EFI_SYSTEM_TABLE*   SystemTable;
    EFI_BOOT_SERVICES   BootServices;
} EFI_CONTEXT;


extern EFI_CONTEXT efi_context;
