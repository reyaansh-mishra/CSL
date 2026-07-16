#pragma once
#include <Uefi.h>

#define CSL_VERSION "0.0.1"


typedef struct {
    EFI_HANDLE          this;
    EFI_SYSTEM_TABLE*   SystemTable;
} EFI_CONTEXT;

extern EFI_CONTEXT efi_context;
