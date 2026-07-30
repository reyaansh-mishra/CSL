#include <csl.h>
#define MAX_ALLOC_REGIONS   512

struct BLOCK {
    void*   addr;           // Base of allocation arena
    size_t  max_size;       // Total arena size
    size_t  used_size;      // Total bytes currently allocated
    size_t  cursor;         // Offset of next search position
};

class BlockAllocator {
    public:
        void    init(void* block_alloc_addr, size_t max_size);
        void*   malloc(size_t pahes);
        void    dealloc(/* void* ptr */);
    private:
        struct BLOCK        block;
};

extern BlockAllocator   allocator;
