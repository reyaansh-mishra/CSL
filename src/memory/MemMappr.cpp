#include "Uefi/UefiBaseType.h"
#include <utils.hpp>

struct MemMapprInfo MemMapprInfo;

void mem_map_init() {
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
        return;
    }

    memory_map_size += 2 * descriptor_size; // pad, common convention

    void* alloc_addr = mem_alloc(memory_map_size);
    if (alloc_addr == NULL) {
        ERR("MemMappr: allocation failed\n");
        return;
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
        print(status);
        print("\n");
        return;
    };

    /* If all above succeded, NOW commit to struct */

    MemMapprInfo.descriptor_size    = descriptor_size;
    MemMapprInfo.descriptor_version = descriptor_version;
    MemMapprInfo.memory_map_size    = memory_map_size;
    MemMapprInfo.memory_map         = (EFI_MEMORY_DESCRIPTOR *)alloc_addr;
    MemMapprInfo.map_key            = map_key;

    INFO("Initialized MemMappr!\n");
};

struct MemMapprInfo getMemMap() {
    return MemMapprInfo;
};