#include <csl.h>
#include <utils.hpp>

#ifdef CSL_FAKE_PAYLOAD_TEST
#include <specific-includes/payload.h>
#endif

void bootstrappr(struct MemMapprInfo mem_info) {   /* Bootstrappr is used to bootstrap the PAYLOAD, not CSL. */
    payload_main();
    return;
};