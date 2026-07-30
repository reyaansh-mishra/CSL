#include <utils.hpp>

#undef INFO
#undef ERR
#define INFO(string)    print("[CSL] <BlockAllocator>: %s", string)
#define ERR(string)     print("[ERR] [CSL] <BlockAllocator>: %s", string)

struct ALLOCD_REGIONS   allocd_regions[512];
size_t                  alloc_regions_ctr   = 0;

/* ----------------------------------------------------------------------- */
/* INIT FUNCTIONS */
/* ----------------------------------------------------------------------- */


void BlockAllocator::init(void* block_alloc_addr, size_t max_pages)
{
    block.addr      = block_alloc_addr;
    block.max_size  = max_pages*CSL_PAGE_SIZE;
    block.used_size = 0;

    block.cursor    = 0;
};

/* ----------------------------------------------------------------------- */
/* HELPER FUNCTIONS */
/* ----------------------------------------------------------------------- */

static int get_me_first_free_alloc_region(size_t size)
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

void* BlockAllocator::malloc(size_t page)
{

    if (alloc_regions_ctr >= MAX_ALLOC_REGIONS) {
        ERR("Out of allocation metadata entries\n");
        return nullptr;
    }

    size_t size = page*CSL_PAGE_SIZE;
    
    if (block.used_size + size > block.max_size) {
        print("BlockAllocator: Out Of Mem: %lu + %lu >= %lu; Retrying with using Dealloc'd Regions.\n", block.used_size, size, block.max_size);
        
        int free_region = get_me_first_free_alloc_region(size);
        if (free_region == -NO_AVAIL_ENTRIES) {
            ERR("Nothing is genuinly avail. Sorry!\n");
            return nullptr;
        }
        else {
            allocd_regions[free_region].currently_allocd = true;
            return allocd_regions[free_region].base;    // No need to overwrite entries
        };
    };

    uint64_t cursor = block.cursor;
    print("MALLOC: cursor: %lu, block.addr: %lx, block.used_size: %d\n", cursor, block.addr, block.used_size);

    block.used_size += size;
    block.cursor    += size;
    
    allocd_regions[alloc_regions_ctr].base              = (void*)((uint64_t)block.addr + (uint64_t)cursor);
    allocd_regions[alloc_regions_ctr].size              = size;
    allocd_regions[alloc_regions_ctr].currently_allocd  = true;  /* COMMIT */

    alloc_regions_ctr++;

    return (void*)((uint64_t)block.addr + cursor);
};

void BlockAllocator::dealloc(void* ptr)
{
    for (size_t i = 0; i < alloc_regions_ctr; i++) {
        if (allocd_regions[i].base == ptr) {
            block.used_size -= allocd_regions[i].size;
            allocd_regions[i].currently_allocd = false;
            return;
        };
    };
};
