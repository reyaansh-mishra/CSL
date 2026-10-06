/* src/memory/MemMappr.cpp */

#include <core.h>
#include <utils.h>
#include <memory.h>
#include <terminal.h>
#include <mem_map.h>
#include <return_codes.h>

UEFI_MEMORY_MAP MemMapprInfo = {0, 0, 0, 0, 0, 0};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  printf("[CSL] <MemMappr>: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   printf("[ERR] [CSL] <MemMappr>: " fmt, ##__VA_ARGS__)


int mem_map_init() {
    INFO("mem map\n");
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

    memory_map_size += descriptor_size; // No pad

    void* alloc_addr = (void *)malloc(memory_map_size).start;
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
        ERR("\nMemMappr.cpp: mem_map_init: status = efi.SystemTable->BootServices->GetMemoryMap( #2: Failed Alloc with Code: %lu\n", (uint64_t)status);
        return -ERR_ALLOC_FAILED;
    };

    /* If all above succeded, NOW commit to struct */

    MemMapprInfo.descriptor_size    = descriptor_size;
    MemMapprInfo.descriptor_version = descriptor_version;
    MemMapprInfo.memory_map_size    = memory_map_size;
    MemMapprInfo.memory_map         = (EFI_MEMORY_DESCRIPTOR *)alloc_addr;
    MemMapprInfo.map_key            = map_key;

    INFO("Saved UEFI Memory Map!\n");
    return SUCCESS;
};

UEFI_MEMORY_MAP getMemMap() {
    if (!MemMapprInfo.active) {
        MemMapprInfo.active = true;
        int err = mem_map_init();
        if (err) {
            ERR("MemMapInit: %d", mem_map_init());
            MemMapprInfo.active = false;
        };
    }
    return MemMapprInfo;
};
