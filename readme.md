#### Current CSL Version:   0.0.1
# Common System Loader
A Personal project for my Low Level Projects
All multi-byte fields are little-endian.
AArch64/ARM64 only

## Target Architecture

- AArch64
- ARMv8-A (minimum: ARMv8.0-A)
- Little-endian

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
- The Payload is **PHYSICALLY** executing from a fixed load address defined by UEFI during boot
- A valid stack has been established.
- SP is 16-byte aligned in accordance with the AArch64 Procedure Call Standard.
- The MMU is enabled using the translation tables constructed by CSL.
- .bss has been zero-initialized.
- .data has been initialized.
- The CPU is executing at EL2 OR EL1 based on Payload-Set config.
- Interrupts may be disabled based on Payload-Set Config.

### Current Limitations

- Payload Address Space Layout Randomization (KASLR) is not implemented.
- Only little-endian systems are supported.

## Payload Lifecycle:
1. csl_bootstrap()  -> Setup Core Runtimes & POST all the EFI System Details to Underlying Subsystems.
2. payload_init()   -> Tell CSL about Virtual Address Mappings, Exception Vectors, etc. MUST CALL `csl_main()`
3. csl_main()       -> Initialize CSL & Actually Setup Whatever `payload_init()` Requested.
4. payload_main()   -> Actual Payload Entry Point. Can Assume Whatever above Section(s) Specify

## Protocol Versions

### v1
- Boot info version
- RAM base
- RAM size