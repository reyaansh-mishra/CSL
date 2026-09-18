/* includes/csl.h */

#pragma once
#include <Uefi.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define CSL_VERSION             "1.0.1"
#define CSL_VERSION_DESC        "Cleanup unnecessary C++ Source Files"
#define CSL_PAGE_SIZE           4096
#define CSL_STACK_SIZE          CSL_PAGE_SIZE*4

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
extern  bool        pls_use_malloc_now;  

/* RETURN CODES */
#define SUCCESS                 0
#define ERR_UNKNOWN             1
#define ERR_ALLOC_FAILED        2
#define ERROR_NO_MEMORY         3
#define ERROR_ALREADY_MAPPED    4
#define ERROR_INVALID_ARG       5
#define NO_AVAIL_ENTRIES        6
