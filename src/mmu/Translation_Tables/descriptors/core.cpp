/* src/mmu/Translation_Tables/descriptors/core.cpp */

#include <utils.hpp>
#include <mmu/page_and_table_descriptor.hpp>
#include <mmu/page_descriptor_helper.hpp>

void GenericDescriptor::init() {
    raw = 0;
};

void GenericDescriptor::reset() {
    raw = 0;
};

bool GenericDescriptor::is_active() const {
    return get_bit(raw, 0);
};

bool GenericDescriptor::is_block() const {
    return get_bit(raw, 1);
};

bool GenericDescriptor::is_page_desc() const {
    return get_bit(raw, 1);
};
