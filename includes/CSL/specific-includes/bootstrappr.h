/* includes/CSL/bootstrappr.hpp */

#include <csl.h>
#include <uefi/mem-map.h>

void bootstrappr(UEFI_MEMORY_MAP mem_info);
[[noreturn]] void csl_continue_if_needed();

extern size_t number_of_pages;
