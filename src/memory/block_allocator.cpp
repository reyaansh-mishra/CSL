/* src/memory/block_allocator.cpp */

#include <stdint.h>
#include <utils.hpp>

#include <memory/block_allocator.hpp>
extern "C" {
    #include <terminal.h>
    #include <memory.h>
    #include <mmu.h>
    #include <return_codes.h>
    #include <Uefi.h>
    #include <memory/block_alloc.h>
};

#define BLCK_ALLOCATOR_INIT_PAGES   3000

#undef INFO
#undef ERR
#define INFO(fmt, ...)  printf("[CSL] <block allocator>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <block allocator>: " fmt, ##__VA_ARGS__)

struct ALLOCD_REGIONS   allocd_regions[512];
size_t                  alloc_regions_ctr   = 0;
size_t                  live_alloc_count    = 0;

BlockAllocator allocator;

/* ----------------------------------------------------------------------- */
/* INIT FUNCTIONS */
/* ----------------------------------------------------------------------- */


void BlockAllocator::init(void* block_alloc_addr, size_t max_pages)
{
    block.addr      = block_alloc_addr;
    block.max_size  = max_pages*PAGE_SIZE;
    block.used_size = 0;

    block.cursor    = 0;
    alloc_regions_ctr = 0;
};

/* ----------------------------------------------------------------------- */
/* HELPER FUNCTIONS */
/* ----------------------------------------------------------------------- */

static int  get_me_first_free_alloc_region(size_t size)
{
    for (size_t i = 0; i < alloc_regions_ctr; i++) {
        if ((allocd_regions[i].currently_allocd == false) && (allocd_regions[i].size >= size)) {
            return i;
        };
    };
    return -NO_AVAIL_ENTRIES;
};

/* ----------------------------------------------------------------------- */
/* CORE FUNCTIONS */
/* ----------------------------------------------------------------------- */

void* BlockAllocator::malloc(size_t size)
{

    if (alloc_regions_ctr >= MAX_ALLOC_REGIONS) {
        ERR("Out of allocation metadata entries\n");
        return nullptr;
    }
    
    if (block.cursor + size > block.max_size) {
        INFO("BlockAllocator: Out Of Mem: %lu + %lu >= %lu; Retrying with using Dealloc'd Regions.\n", block.used_size, size, block.max_size);
        
        int free_region = get_me_first_free_alloc_region(size);
        if (free_region == -NO_AVAIL_ENTRIES) {
            ERR("Nothing is genuinly avail. Sorry!\n");

            while (true) {
                __asm__ volatile("yield");
            }
            return nullptr;
        }
        else {
            allocd_regions[free_region].currently_allocd = true;
            live_alloc_count++;
            memset(allocd_regions[free_region].base, 0, allocd_regions[free_region].size);
            return allocd_regions[free_region].base;    // No need to overwrite entries' metadata
        };
    };

    uint64_t cursor = block.cursor;

    block.used_size += size;
    block.cursor    += size;
    
    allocd_regions[alloc_regions_ctr].base              = (void*)((uint64_t)block.addr + (uint64_t)cursor);
    allocd_regions[alloc_regions_ctr].size              = size;
    allocd_regions[alloc_regions_ctr].currently_allocd  = true;  /* COMMIT */

    memset(allocd_regions[alloc_regions_ctr].base, 0, allocd_regions[alloc_regions_ctr].size);

    alloc_regions_ctr++;
    live_alloc_count++;

    return (void*)((uint64_t)block.addr + cursor);
};

void BlockAllocator::dealloc(void* ptr)
{
    for (size_t i = 0; i < alloc_regions_ctr; i++) {
        if (allocd_regions[i].base == ptr) {
            block.used_size -= allocd_regions[i].size;
            allocd_regions[i].currently_allocd = false;
            live_alloc_count--;

            if (live_alloc_count == 0) {
                allocator.init(block.addr, block.max_size/PAGE_SIZE);
            };

            return;
        };
    };
};

void malloc_init() {
    INFO("ENABLING CSL BLOCK ALLOCATOR WITH NUMBER OF PAGES: %d\n", BLCK_ALLOCATOR_INIT_PAGES);
    uintptr_t block = (uintptr_t)malloc(BLCK_ALLOCATOR_INIT_PAGES).start;

    map_pages(
        {block,BLCK_ALLOCATOR_INIT_PAGES},
        {block, BLCK_ALLOCATOR_INIT_PAGES},
        WRITABLE
    );

    allocator.init((void* )block, BLCK_ALLOCATOR_INIT_PAGES);
};

uintptr_t block_alloc(size_t size) {
    return (uintptr_t)allocator.malloc(size);
};

void block_free(uintptr_t ptr) {
    allocator.dealloc((void *)ptr);
};
