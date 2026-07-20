#include <csl.h>

#ifndef BUILD_FOR_AMD64
uint32_t get_current_el(void);
extern "C" void not_in_el2(void);
#endif