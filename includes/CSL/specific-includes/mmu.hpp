/* includes/CSL/specific-includes/mmu.hpp */

#pragma once
#include <csl.h>

class Table_Descriptor {
    public:
        void init();
        void clear();
        void set_next_table(uintptr_t ptr_to_table);
        void set_valid(bool);
        bool is_valid() const;
        void validate();        // One-Shot

        uintptr_t get_next_level();

    private:
        uint64_t raw = 0;
};

class Page_Descriptor {
    public:
        void        init();
        void        clear();
        void        set_valid(bool);
        bool        is_valid() const;
        void        validate(struct L3_Page_Descriptor_Info table, uintptr_t phy_addr);        // One-Shot

        uintptr_t get_page_addr();

        void set_mair();

    private:
        void setup_L3_table(struct L3_Page_Descriptor_Info minimal_table_info);
        uint64_t raw = 0;
};

inline void set_bit(uint64_t& target, uint64_t bit, bool val)
{
    if (val)
        target |= (1ULL << bit);
    else
        target &= ~(1ULL << bit);
};

inline bool get_bit(uint64_t target, uint64_t bit)
{
    return ((target >> bit) & 1ULL);
};

extern Table_Descriptor L1_table[512];

void setup_tables();

extern "C" void write_mair(uint64_t data);
extern "C" void write_tcr(uint64_t data);
extern "C" void write_ttbr0(uint64_t data);
extern "C" void enable_mmu();
extern "C" void disable_mmu();
