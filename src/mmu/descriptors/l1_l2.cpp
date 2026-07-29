/* src/mmu/descriptors/l1_l2.cpp */

#include <utils.hpp>
#define FIRST_ADDR_TABLE_BITS               38
#define INFORMATIVE_TABLE_ENTRIES_IN_RAW    2 + FIRST_ADDR_TABLE_BITS

/** INFO: 
 * [63:59]  -> ATTRIBUTES
 * [58:51]  -> IGNORED
 * [50]     -> RES0
 * [49:12]  -> NEXT TABLE ADDR (from addr = [49:12])
 * [11:10]  -> IGNORED
 * [9:8]    -> NEXT TABLE ADDR (from addr = [51:50])
 * [7:2]    -> IGNORED 
 * [1]      -> VAL 1
 * [0]      -> VAL 1
 */

/* ----------------------------------------------------------------------- */
/* HELPERS */
/* ----------------------------------------------------------------------- */


bool Table_Descriptor::is_valid() const
{
    if (get_bit(raw, 1) && get_bit(raw, 0)) { return true; } else { return false; };
};

void Table_Descriptor::clear() { raw = 0; };

void Table_Descriptor::validate()    // One-shot
{
    set_bit(raw, 0, 1);

};

void Table_Descriptor::set_valid(bool state)
{
    set_bit(raw, 0, state);

};

/* ----------------------------------------------------------------------- */
/* CORE FUNCTIONS */
/* ----------------------------------------------------------------------- */


void Table_Descriptor::init() /* Make sure Table Descriptor is in a ""State"" */
{
    raw = 0;

    set_bit(raw, 1, 1);
};


void Table_Descriptor::set_next_table(uintptr_t addr)   // Expects the physical base address of an L2/L3 table.
{

    for (size_t i = 0; i < FIRST_ADDR_TABLE_BITS; i++) {
        raw &= ~(1ULL << (49 - i));
    };

    for (size_t i = 0; i < 2; i++) {
        raw &= ~(1ULL << (9 - i));
    };


    for (size_t i = 0; i < FIRST_ADDR_TABLE_BITS; i++) {
        bool bit = get_bit(addr, 49 - i);   // Goes down to 11
        set_bit(raw, 49 - i, bit);
    };

    for (size_t i = 0; i < 2; i++) {                            // Run EXACTLY twice. Ill keep it like this for Consistency.
        size_t target_bit   = 8 + i;                            // Get the value of the target bit to change
        size_t from_bit     = 50+i;                             // Get the value of the bit we want to change it to
        bool bit = get_bit(addr, from_bit); // Offset by 8
        set_bit(raw, target_bit, bit);
    };

    return;
};

uintptr_t Table_Descriptor::get_next_level()
{
    uintptr_t bits_8_9      = 0;
    uintptr_t bits_12_49    = 0;
    uintptr_t bit_final     = 0;

    uintptr_t bit_8 = get_bit(raw, 8);
    uintptr_t bit_9 = get_bit(raw, 9);

    set_bit(bits_8_9, 0, bit_8);
    set_bit(bits_8_9, 1, bit_9);

    for (size_t i = 0; i < FIRST_ADDR_TABLE_BITS; i++) {
        set_bit(bits_12_49, i, get_bit(raw, i+12));
    };

    for (size_t i = 0; i < INFORMATIVE_TABLE_ENTRIES_IN_RAW; i++) {
        if (i < 2) {    // Get from bits_8_9
            set_bit(bit_final, i + 50, get_bit(bits_8_9, i));    // Starting bits [0:11] are ZERO && Start at 50
        } else {        // Starts at 2
            set_bit(bit_final, i + 10, get_bit(bits_12_49, i-2)); // Start at 12 && Offset to Zero
        };
    };

    return bit_final;
};
