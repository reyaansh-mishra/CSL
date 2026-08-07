/* includes/payload-includes/payload.h */

#include <csl.h>

/* ------------------------------------------------------------ */
/* Stuff for PAYLOAD */
/* ------------------------------------------------------------ */

#define PAYLOAD_MAX_REMAP_ADDRS 32

enum VIRT_ADDR_PERMISSIONS {
    READ_ONLY   = 1 << 0,   /* To be used ONLY FOR INIT. Otherwise will Error out if active == true. */
    WRITABLE    = 1 << 1,
    EXECUTABLE  = 1 << 2,
};

struct PAYLOAD_REMAP_ADDRS {
    uintptr_t   phy_start_addr;
    uintptr_t   virt_start_addr;

    size_t      size;

    enum VIRT_ADDR_PERMISSIONS virtual_addr_permissions;

    bool active;        /* Just-In-Case Check to make sure something bad doesnt happen */
};

struct PAYLOAD_BOOT_INFO {
    uintptr_t   ImageBase;
    uint64_t    ImageSize;
    char*       BootArgs;
} __attribute__((packed));


extern struct   PAYLOAD_BOOT_INFO   boot_info;
extern struct   PAYLOAD_REMAP_ADDRS remap_addrs[PAYLOAD_MAX_REMAP_ADDRS];
extern uint8_t                      remap_addrs_count;
extern uintptr_t                    payload_virtual_entry;
extern uint64_t                     payload_reloc_physically;

/* FUNCTIONS */

void add_virtual_mapping(uintptr_t phy_start_addr, uintptr_t virt_start_addr, size_t size, enum VIRT_ADDR_PERMISSIONS permissions);
void payload_main(struct PAYLOAD_BOOT_INFO boot_struct);
