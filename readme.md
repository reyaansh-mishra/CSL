#### Current CSL Version:   0.0.1
# Common System Loader
A Personal project for my Low Level Projects
AArch64/ARM64 only

## Target Architecture

- AArch64
- ARMv8-A

## Goals:
1. Prepare a minimal execution environment such that the Payload can begin executing C/C++ code immediately.
3. Load Initrd if needed (LATER_GOAL)
4. Transfer control to the Payload.

## Non-Goals:
1. Anything not **EXPLICITLY** stated in Goals.

---

## What does the Payload Expect?

### Payload Entry ABI

> ANYTHING THAT ISNT GUARANTEED HERE IS NOT TO BE ASSUMED

On entry to the Payload:

- The Payload is provided a BOOT_INFORMATION struct as its entry arg
- The Payload is **PHYSICALLY** executing from a fixed load address defined by **UEFI** during boot
- The MMU is enabled using the translation tables constructed by CSL, as defined by Payload (otherwise full identity map).
- The CPU is executing at EL2 OR EL1 based on Payload-Set config.
- Interrupts are be disabled.

## Payload Lifecycle:
1. csl_bootstrap()  -> Setup Core Runtimes & POST all the EFI System Details to Underlying Subsystems.
2. payload_init()   -> Tell CSL about Virtual Address Mappings, Exception Vectors, etc. MUST CALL `csl_main()`
3. csl_main()       -> Initialize CSL & Actually Setup Whatever `payload_init()` Requested.
4. payload_main()   -> Actual Payload Entry Point. Can Assume Whatever above Section(s) Specify

## Quick Info on Certain Decisions:
### Why are Interrupts ALWAYS Disabled?
- I categorize CSL + payload_main() as a "critical phase" - payload_main() is still the payload bringing itself up. Its not the full OS.
- "Exception Vectors Exists" != "Safe to Unmask"
Hence, CSL Should make sure that Interrupts are disabled, even though we may have installed the Exception Vectors.

### No AMD64?
- I wont really be developing for AMD64 on my personal projects. Hence No Support.

## Protocol Versions for Boot Info Struct

### v1
1. uint8_t  boot_info_version
2. uint64_t ram_phy_base
3. size_t   ram_size
4. __attribute__((packed))
