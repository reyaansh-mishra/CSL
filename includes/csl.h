#pragma once
#include <Uefi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define CSL_VERSION             "0.0.1"
#define CSL_FAKE_PAYLOAD_TEST
#define CSL_PAGE_SIZE           4096
#define PAYLOAD_MAX_REMAP_ADDRS

typedef struct {
    EFI_HANDLE          ImageHandle;
    EFI_SYSTEM_TABLE*   SystemTable;
    EFI_BOOT_SERVICES*  BootServices;
} EFI_CONTEXT;

extern                      EFI_CONTEXT efi;

/* ------------------------------------------------------------ */
/* Stuff for PAYLOAD */
/* ------------------------------------------------------------ */


enum VIRT_ADDR_PERMISSIONS {
    WRITABLE    = 1 << 0,
    EXECUTABLE  = 1 << 1,
};

typedef struct {
    uintptr_t   phy_start_addr;
    uintptr_t   virt_start_addr;

    size_t      size;

    enum VIRT_ADDR_PERMISSIONS virtual_addr_permissions;

    bool active;        /* Just-In-Case Check for Bootstrappr to make sure something bad doesnt happen */
} PAYLOAD_REMAP_ADDRS;

extern PAYLOAD_REMAP_ADDRS  remap_addrs[PAYLOAD_MAX_REMAP_ADDRS];
extern size_t               remap_addrs_count;

extern uintptr_t            payload_virtual_entry;
