#include <mmu/orchestrator_required.h>
#include <memory.h>

struct MemMapprInfo memory_map;

static void reinit_mem_map()
{
    memory_map   = getMemMap();
};

void setup_tables_as_needed()
{
    reinit_mem_map();
};
