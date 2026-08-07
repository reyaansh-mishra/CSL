/* includes/CSL/utils.hpp */

#pragma once
#include <csl.h>

#include <specific-includes/terminal.hpp>

#define INFO(fmt, ...)    print("[CSL]: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)     print("[ERR] [CSL]: " fmt, ##__VA_ARGS__)


extern "C" inline size_t strlen(const char* str);

extern EFI_GUID gEfiLoadedImageProtocolGuid;
extern EFI_GUID gEfiSimpleTextOutProtocolGuid;

#include <specific-includes/arm64.hpp>

#include <specific-includes/memory.hpp>

#include <specific-includes/bootstrappr.hpp>

#include <specific-includes/mmu.hpp>

#include <specific-includes/block_allocator.hpp>

inline uint64_t round_up(uint64_t num, uint64_t to_multiple_of)
{
    if (to_multiple_of == 0) return num;
    return (((num + (to_multiple_of-1))/to_multiple_of) * to_multiple_of);
};

inline uint64_t round_down(uint64_t num, uint64_t to_multiple_of)
{
    if (to_multiple_of == 0) return num;
    return (num / to_multiple_of) * to_multiple_of;
};

#include <specific-includes/page_descriptor_helper.hpp>
