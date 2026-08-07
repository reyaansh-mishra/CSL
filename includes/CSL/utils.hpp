/* includes/CSL/utils.hpp */

#pragma once
#include <csl.h>

#define INFO(fmt, ...)    print("[CSL]: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)     print("[ERR] [CSL]: " fmt, ##__VA_ARGS__)

extern EFI_GUID gEfiLoadedImageProtocolGuid;
extern EFI_GUID gEfiSimpleTextOutProtocolGuid;

static inline uint64_t round_up(uint64_t num, uint64_t to_multiple_of)
{
    if (to_multiple_of == 0) return num;
    return (((num + (to_multiple_of-1))/to_multiple_of) * to_multiple_of);
};

static inline uint64_t round_down(uint64_t num, uint64_t to_multiple_of)
{
    if (to_multiple_of == 0) return num;
    return (num / to_multiple_of) * to_multiple_of;
};

static inline size_t strlen(const char* str);
