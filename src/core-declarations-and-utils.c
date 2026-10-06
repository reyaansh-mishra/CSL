/* src/core-declerations-and-utils.c */

#include <stdbool.h>
#include <stdint.h>

#include <csl_lib.h>
#include <memory.h>
#include <UEFI_CONTEXT.h>
#include <SYSTEM_STATE.h>

UEFI        uefi_context            = {0};
bool        uefi_died               = false;

malloc_provider_t   uefi_malloc_provider    = 0;
free_provider_t   uefi_free_provider      = 0;

void set_uefi_malloc(malloc_provider_t ptr) {
    uefi_malloc_provider = ptr;
};

void set_free_malloc(free_provider_t ptr) {
    uefi_free_provider = ptr;
};
