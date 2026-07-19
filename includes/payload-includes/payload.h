#include <csl.h>

/* ------------------------------------------------------------ */
/* Stuff for PAYLOAD */
/* ------------------------------------------------------------ */

#define PAYLOAD_MAX_REMAP_ADDRS 32

enum VIRT_ADDR_PERMISSIONS {
    NONE        = 1 << 0,   /* To be used ONLY FOR INIT. Otherwise will Error out if active == true. */
    WRITABLE    = 1 << 1,
    EXECUTABLE  = 1 << 2,
};

struct PAYLOAD_REMAP_ADDRS {
    uintptr_t   phy_start_addr;
    uintptr_t   virt_start_addr;

    size_t      size;

    enum VIRT_ADDR_PERMISSIONS virtual_addr_permissions;

    bool active;        /* Just-In-Case Check for Bootstrappr to make sure something bad doesnt happen */
};

extern struct PAYLOAD_REMAP_ADDRS   remap_addrs[PAYLOAD_MAX_REMAP_ADDRS];
extern uint8_t                      remap_addrs_count;

extern uintptr_t                    payload_virtual_entry;

extern bool                         exit_boot_services;

/* FUNCTIONS */

void add_virtual_mapping(uintptr_t phy_start_addr, uintptr_t virt_start_addr, size_t size, enum VIRT_ADDR_PERMISSIONS permissions);
