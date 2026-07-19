#pragma once
#include <Uefi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define CSL_VERSION             "0.0.1"
#define CSL_FAKE_PAYLOAD_TEST
#define CSL_PAGE_SIZE           4096

typedef struct {
    EFI_HANDLE          ImageHandle;
    EFI_SYSTEM_TABLE*   SystemTable;
    EFI_BOOT_SERVICES*  BootServices;
} EFI_CONTEXT;

extern                      EFI_CONTEXT efi;
