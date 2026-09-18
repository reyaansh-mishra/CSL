#include <payload-includes/payload.h>
#include <utils.h>
#include <terminal.h>

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[PAXON CSL INITTIALIZER]: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [PAXON CSL INITTIALIZER]: " fmt, ##__VA_ARGS__)


int payload_init() {
    INFO("PAXON! INIT\n");
    csl_main();
    return 0;
};

#undef INFO
#undef ERR
#define INFO(fmt, ...)  print("[LibFS]: " fmt, ##__VA_ARGS__)
#define ERR(fmt, ...)   print("[ERR] [LibFS]: " fmt, ##__VA_ARGS__)


void payload_main(struct PAYLOAD_BOOT_INFO __attribute__((unused)) boot_struct) {
    INFO("fmt, ...\n");
};
