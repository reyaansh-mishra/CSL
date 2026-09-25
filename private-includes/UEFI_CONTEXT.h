/* private-includes/EFI_CONTEXT.h */

#include <Uefi.h>
#include <stddef.h>
#include <stdint.h>

typedef struct EFI_CONTEXT {
    EFI_HANDLE          ImageHandle;
    EFI_SYSTEM_TABLE*   SystemTable;
    EFI_BOOT_SERVICES*  BootServices;
} UEFI;
extern UEFI uefi_context;
