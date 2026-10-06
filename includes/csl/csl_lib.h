/* includes/csl/csl_lib.h */

#include <stdint.h>
#include <memory.h>

/** 
  * THIS FILE HOUSES THE CORE CSL LIB INCLUDES.
  * THESE FUNTIONS DIRECTLY MODIFY CSL's INTERNAL STATE.
  */

void set_uefi_malloc(malloc_provider_t ptr);
void set_free_malloc(free_provider_t ptr);
