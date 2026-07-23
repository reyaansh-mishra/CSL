/* includes/CSL/arm64.hpp */

#include <csl.h>

uint32_t get_current_el(void);
extern "C" void not_in_el2(void);

extern "C" void mask_interrupts();
extern "C" void unmask_interrupts();    // Payload Utility
extern "C" void mask_FULL();

extern "C" void write_vbar_el2(uintptr_t a);
extern "C" void vector_table();

extern "C" inline void install_vbar() {
    write_vbar_el2((uintptr_t)&vector_table);
};
