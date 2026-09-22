.global SYSTEM_POWEROFF
.global SYSTEM_HALT

/* AI GENERATED */

SYSTEM_POWEROFF:
    // 1. Load the PSCI SYSTEM_OFF Function ID into x0
    // ID: 0x84000008 (SMC32 convention used for system-wide commands)
    ldr     x0, =0x84000008
    
    // 2. Trigger the firmware call
    smc     #0

// If SMC fails or returns, trap the CPU in a low-power infinite loop
trap:
    wfi
    b       trap

/* END AI GENERATED */

SYSTEM_HALT:
    wfi
    b SYSTEM_HALT
