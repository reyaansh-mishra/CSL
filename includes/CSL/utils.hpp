/* includes/CSL/utils.hpp */

#pragma once

extern "C" {
    #include <utils.h>
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
