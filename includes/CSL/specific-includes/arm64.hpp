/* includes/CSL/arm64.hpp */

#include <csl.h>

#ifndef BUILD_FOR_AMD64
uint32_t get_current_el(void);
extern "C" void not_in_el2(void);

extern "C" void mask_interrupts();
extern "C" void unmask_interrupts();    // Payload Utility

#endif
