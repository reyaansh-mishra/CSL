/* includes/CSL/specific-includes/uefi/mem-map.h */

#pragma once
#include <csl.h>

struct MemMapprInfo {
    EFI_MEMORY_DESCRIPTOR*  memory_map;
    UINTN                   memory_map_size;
    UINTN                   map_key;
    UINTN                   descriptor_size;
    UINT32                  descriptor_version;

    bool active;
};
typedef struct MemMapprInfo UEFI_MEMORY_MAP;
