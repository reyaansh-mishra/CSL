/* includes/CSL/utils.h */

#pragma once
#include <csl.h>

#define ABSOLUTE_LINKER_PREF_ADDR   0x140000000

#define INFO(fmt, ...)                      print("[CSL]: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)                       print("[ERR] [CSL]: " fmt, ##__VA_ARGS__)
#define ASSERT(args) \
    do { \
        if (!(args)) { \
            ERR("ASSERT FAILED: %s\n", #args); \
            __asm__ volatile("wfi"); \
        } \
    } while (0)

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

static inline bool get_bit(uint64_t source, uint64_t bit) {
    return ((source >> bit) & 1ULL);
};

static inline uint64_t get_bits(uint64_t src, size_t start_bit, size_t end_bit) {  // start_bit -> end_bit
    return (src >> end_bit) & ((1ULL << (start_bit - end_bit + 1)) - 1);
};

#ifndef __cplusplus
static inline void set_bit(uint64_t* target, uint64_t bit, bool val) {
    if (!target) { return; };

    if (val)
        *target |= (1ULL << bit);
    else
        *target &= ~(1ULL << bit);
};
#endif
