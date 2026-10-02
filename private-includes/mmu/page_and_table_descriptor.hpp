/* private-includes/mmu/page_and_table_descriptor.hpp */

#pragma once

extern "C" {
    #include <core.h>
    #include <utils.h>
};

class GenericDescriptor {
    public:
        void init();
        void reset();
        bool is_active() const;
        bool is_page_desc() const;

        bool is_block() const;

    protected:
        uint64_t raw = 0;
};

class Table_Descriptor : public GenericDescriptor {
    public:
        void        activate(uintptr_t ptr_to_next_table);        // One-Shot
        uintptr_t   get_next_table();
    private:
        void        set_next_table(uintptr_t ptr_to_next_table);
};

class Page_Descriptor : public GenericDescriptor {
    public:
        void        activate(struct Descriptor_Info table, uintptr_t phy_addr);        // One-Shot
        void        set_mair();
        uintptr_t   get_page();

    private:
        void        setup_table(struct Descriptor_Info minimal_table_info);
};

class L2_final : public Page_Descriptor {
    public:
        void    activate(struct Descriptor_Info table, uintptr_t phy_addr);        // One-Shot
};

class L1_final : public Page_Descriptor {
    public:
        void    activate(struct Descriptor_Info table, uintptr_t phy_addr);        // One-Shot
};

extern Table_Descriptor __attribute__((aligned(PAGE_SIZE))) L0_table[512];
extern "C" void setup_tables();
