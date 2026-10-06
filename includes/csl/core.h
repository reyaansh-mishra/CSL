/* includes/core.h */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <Uefi.h>

#define PAGE_SIZE   4096
#define CSL_VERSION             "1.0.2"
#define CSL_VERSION_DESC        "Re"

typedef struct {
    EFI_HANDLE          ImageHandle;
    EFI_SYSTEM_TABLE*   SystemTable;
    EFI_BOOT_SERVICES*  BootServices;

    uintptr_t csl_base_phy;
    uintptr_t csl_base_virt;
    uint64_t  csl_size;

    uintptr_t   ram_base;
    size_t      ram_size;
} EFI_CONTEXT;

extern  EFI_CONTEXT efi;

struct SET_OF_PAGES {
    uintptr_t   start;
    size_t      num_of_pages;
};
typedef struct SET_OF_PAGES PAGES;
