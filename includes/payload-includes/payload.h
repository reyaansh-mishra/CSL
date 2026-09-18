/* includes/payload-includes/payload.h */

#pragma once
#include <csl.h>

#include <uefi/mem-map.h>

/* ------------------------------------------------------------ */
/* Stuff for PAYLOAD */
/* ------------------------------------------------------------ */

#define PAYLOAD_MAX_REMAP_ADDRS 32

enum VIRT_ADDR_PERMISSIONS {
    READ_ONLY   = 1 << 0,
    WRITABLE    = 1 << 1,
    EXECUTABLE  = 1 << 2,

    DEVICE      = 1 << 3,
};

struct PAYLOAD_REMAP_ADDRS {
    uintptr_t   phy_start_addr;
    uintptr_t   virt_start_addr;

    size_t      size;

    enum VIRT_ADDR_PERMISSIONS virtual_addr_permissions;

    bool active;        /* Just-In-Case Check */
};
extern struct PAYLOAD_REMAP_ADDRS remap_addrs[PAYLOAD_MAX_REMAP_ADDRS];

struct PAYLOAD_BOOT_INFO {
    uint8_t     version;
    uintptr_t   ImageBase_phy;
    uintptr_t   ImageBase_virt;
    size_t      ImageSize;

    UEFI_MEMORY_MAP uefi_memory_map;

    char*       BootArgs;
} __attribute__((packed));
extern struct PAYLOAD_BOOT_INFO   boot_info;

extern uint8_t      remap_addrs_count;
extern uintptr_t    payload_virtual_entry;
extern uint64_t     payload_reloc_physically;
extern bool         drop_to_el1;

/* FUNCTIONS */
void add_virtual_mapping(uintptr_t phy_start_addr, uintptr_t virt_start_addr, size_t size, enum VIRT_ADDR_PERMISSIONS permissions);
void payload_main(struct PAYLOAD_BOOT_INFO boot_struct);
