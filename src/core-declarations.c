#include <UEFI_CONTEXT.h>
#include <SYSTEM_STATE.h>
#include <stdbool.h>

UEFI        uefi_context            = {0};
bool        uefi_died               = false;

uintptr_t   uefi_malloc_provider    = 0;
uintptr_t   uefi_free_provider      = 0;
