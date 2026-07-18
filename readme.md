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
### -> KASLR DOESNT EXIST (LATER_GOAL)

### How it looks:

CSL Boot Information Layout v1

| Offset | Size | Type     | Description           |
|--------|------|----------|-----------------------|
| 0x00   | 4    | uint32_t | CSL Boot Info Version |
| 0x04   | 8    | uint64_t | RAM Base              |
| 0x0C   | 8    | uint64_t | RAM Size              |
| xxxxxx | xxxx | void*    | EFI (base addr Image) |

### Payload Entry ABI

> ANYTHING THAT ISNT GUARANTEED HERE IS NOT TO BE ASSUMED

On entry to the Payload:

- The Given Boot Information Layout is Fixed for every boot and is where the Payload MUST check.
- The Payload is executing from a fixed load address defined by UEFI during boot
- SP is initialized and 16-byte aligned.
- MMU is enabled.
- .bss has been zero-initialized.
- .data has been initialized.
- The CPU is executing at EL2/EL1 (based on config).
- Interrupts are disabled.

### Current Limitations

- Payload Address Space Layout Randomization (KASLR) is not implemented.
- Only little-endian systems are supported.

## Protocol Versions

### v1
- Boot info version
- RAM base
- RAM size