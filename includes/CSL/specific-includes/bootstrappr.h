/* includes/CSL/bootstrappr.hpp */

#include <csl.h>
#include "memory.h"

void bootstrappr(struct MemMapprInfo mem_info);
[[noreturn]] void csl_continue_if_needed();

extern size_t number_of_pages;
