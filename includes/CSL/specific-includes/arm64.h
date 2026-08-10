/* includes/CSL/arm64.hpp */

#include <csl.h>

uint32_t get_current_el(void);
void not_in_el2(void);

void mask_interrupts();
void unmask_interrupts();    // Payload Utility
void mask_FULL();

void write_vbar_el2(uintptr_t a);
void vector_table();

inline void install_vbar() {
    write_vbar_el2((uintptr_t)&vector_table);
};
