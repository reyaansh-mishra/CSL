/* includes/CSL/specific-includes/mmu.hpp */

#pragma once

extern "C" {
    #include <utils.h>
};

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

extern "C" void setup_tables();
