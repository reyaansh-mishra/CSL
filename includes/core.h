/* includes/core.h */
#pragma once

#include <stdint.h>
#include <stddef.h>
#define PAGE_SIZE   4096

struct SET_OF_PAGES {
    uintptr_t   start;
    size_t      num_of_pages;
};
typedef struct SET_OF_PAGES PAGES;
