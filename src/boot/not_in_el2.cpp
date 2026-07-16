#include "Base.h"
#include <utils.hpp>


extern "C" void not_in_el2(void) {
    print("CSL: NOT IN EL2. BUSY-LOOPING.");

    while (TRUE) {
        __asm__ volatile("wfi");
    };
};