/* includes/CSL/arm64.hpp */

#include <csl.h>

uint32_t get_current_el(void);
void not_in_el2(void);

void mask_interrupts();
void unmask_interrupts();    // Payload Utility
void mask_FULL();

void write_vbar_el2(uintptr_t a);
void vector_table();

static inline void install_vbar() {
    write_vbar_el2((uintptr_t)&vector_table);
};

void write_mair(uint64_t data);
void write_tcr(uint64_t data);
void write_ttbr0(uint64_t data);
void write_hcr(uint64_t data);
uint64_t read_hcr();

void enable_mmu();
void disable_mmu();

static inline uintptr_t get_current_pc() {
    uint64_t current_pc = 0;

    __asm__ volatile(
        "mrs %0, elr_el2"
        : "=r"(current_pc)
        :
        :
    );
    return current_pc;
};

static inline uintptr_t get_current_sp() {
    uint64_t current_sp = 0;

    __asm__ volatile(
        "mov %0, sp"
        : "=r"(current_sp)
        :
        :
    );
    return current_sp;
};

void SYSTEM_POWEROFF();
void SYSTEM_HALT();
