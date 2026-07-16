#include <stdint.h>
#include <utils.hpp>

uint32_t get_current_el(void) {
    uint64_t el_reg;
    
    // Read the CurrentEL system register into an assembly variable
    __asm__ volatile("mrs %0, CurrentEL" : "=r" (el_reg));
    
    // The EL bits are stored in bits [3:2], so shift right by 2
    return (uint32_t)(el_reg >> 2);
};