/* includes/CSL/utils.hpp */

#pragma once
#include <csl.h>

#include <specific-includes/terminal.hpp>

#define INFO(string)    pr_info("[CSL]: ", (string))
#define ERR(string)     pr_info("[ERR] [CSL]: ", (string))


extern "C" inline size_t strlen(const char* str);

#ifndef BUILD_FOR_AMD64
#include <specific-includes/arm64.hpp>
#endif


#include <specific-includes/memory.hpp>

#include <specific-includes/bootstrappr.hpp>
