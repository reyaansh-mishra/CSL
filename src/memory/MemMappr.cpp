/* src/memory/MemMappr.cpp */

#include <utils.hpp>
#include <payload-includes/payload.h>

PAYLOAD_REMAP_ADDRS remap_addrs[PAYLOAD_MAX_REMAP_ADDRS] = {};
uint8_t             remap_addrs_count = 0;

struct MemMapprInfo MemMapprInfo;

int mem_map_init() {
    UINTN memory_map_size = 0;
    EFI_MEMORY_DESCRIPTOR *memory_map = 0;
    UINTN map_key = 0;
    UINTN descriptor_size = 0;
    UINT32 descriptor_version = 0;

    EFI_STATUS status;

    // --- Call #1: pass a size of 0 on purpose, just to get the real size back ---
    status = efi.SystemTable->BootServices->GetMemoryMap(
        &memory_map_size,
        memory_map,        // NULL right now
        &map_key,
        &descriptor_size,
        &descriptor_version
    );
    // This call is EXPECTED to fail with EFI_BUFFER_TOO_SMALL.

    if (status != EFI_BUFFER_TOO_SMALL) {
        ERR("MemMappr.cpp: mem_map_init:     GetMemoryMap probe failed\n");
        return -ERR_UNKNOWN;
    }

    memory_map_size += 2 * descriptor_size; // pad, common convention

    void* alloc_addr = mem_alloc(memory_map_size);
    if (alloc_addr == NULL) {
        ERR("MemMappr: allocation failed\n");
        return -ERR_ALLOC_FAILED;
    }

    // --- Call #2: Actually Call ---
    status = efi.SystemTable->BootServices->GetMemoryMap(
        &memory_map_size,
        (EFI_MEMORY_DESCRIPTOR *)alloc_addr,
        &map_key,
        &descriptor_size,
        &descriptor_version
    );

    if (EFI_ERROR(status)) {
        ERR("\n MemMappr.cpp: mem_map_init:     status = efi.SystemTable->BootServices->GetMemoryMap( #2: Failed Alloc with Code: ");
        print((uint64_t)status);
        print("\n");
        return -ERR_ALLOC_FAILED;
    };

    /* If all above succeded, NOW commit to struct */

    MemMapprInfo.descriptor_size    = descriptor_size;
    MemMapprInfo.descriptor_version = descriptor_version;
    MemMapprInfo.memory_map_size    = memory_map_size;
    MemMapprInfo.memory_map         = (EFI_MEMORY_DESCRIPTOR *)alloc_addr;
    MemMapprInfo.map_key            = map_key;

    INFO("Initialized MemMappr!\n");
    return SUCCESS;
};

struct MemMapprInfo getMemMap() {
    return MemMapprInfo;
};

void add_virtual_mapping(uintptr_t phy_start_addr, uintptr_t virt_start_addr, size_t size, enum VIRT_ADDR_PERMISSIONS permissions)
{
    #undef INFO
    #define INFO(string) pr_info("[PAYLOAD]: ", string)

    if (remap_addrs_count >= PAYLOAD_MAX_REMAP_ADDRS) {
        ERR("PAYLOAD_REMAP_ADDRS full. Refusing mapping.\n");
        return;
    }


    PAYLOAD_REMAP_ADDRS* remap_addr = &remap_addrs[remap_addrs_count];
    remap_addr->phy_start_addr           = phy_start_addr;
    remap_addr->virt_start_addr          = virt_start_addr;
    remap_addr->size                     = size;
    remap_addr->virtual_addr_permissions = permissions;

    remap_addr->active                   = true; /* COMMIT */
    
    remap_addrs_count++;
};