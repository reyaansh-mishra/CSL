#include <utils.hpp>

#undef INFO
#undef ERR
#define INFO(string)    print("[CSL] <BlockAllocator>: %s", string)
#define ERR(string)     print("[ERR] [CSL] <BlockAllocator>: %s", string)


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
/* CORE FUNCTIONS */
/* ----------------------------------------------------------------------- */

void* BlockAllocator::malloc(size_t page)
{
    size_t size = page*CSL_PAGE_SIZE;
    
    if (block.used_size + size >= block.max_size) {
        print("BlockAllocator: Out Of Mem: %lu + %lu >= %lu\n", block.used_size, size, block.max_size);
        return nullptr;
    };

    uint64_t cursor = block.cursor;
    print("MALLOC: cursor: %lu, block.addr: %lx, block.used_size: %d\n", cursor, block.addr, block.used_size);

    block.used_size += size;
    block.cursor    += size;
    
    return (void*)((uint64_t)block.addr + cursor);
};

void BlockAllocator::dealloc(/* void* ptr */)
{
    // TODO
};
