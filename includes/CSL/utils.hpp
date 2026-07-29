/* includes/CSL/utils.hpp */

#pragma once
#include <csl.h>

#include <specific-includes/terminal.hpp>

#define INFO(string)    print("[CSL]: %s", string)
#define ERR(string)     print("[ERR] [CSL]: %s", string)


extern "C" inline size_t strlen(const char* str);

#ifndef BUILD_FOR_AMD64
#include <specific-includes/arm64.hpp>
#endif


#include <specific-includes/memory.hpp>

#include <specific-includes/bootstrappr.hpp>

#include <specific-includes/mmu.hpp>
