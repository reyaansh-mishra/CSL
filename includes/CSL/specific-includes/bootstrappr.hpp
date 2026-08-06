/* includes/CSL/bootstrappr.hpp */

#include <csl.h>

void bootstrappr(struct MemMapprInfo mem_info);
extern "C" [[noreturn]] void csl_continue_if_needed();

extern size_t number_of_pages;
