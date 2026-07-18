#include <utils.hpp>


extern "C" void not_in_el2(void) {
    ERR("NOT IN EL2. BUSY-LOOPING.");

    while (TRUE) {
        __asm__ volatile("wfi");
    };
};
