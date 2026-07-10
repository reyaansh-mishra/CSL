#### Current CSL Version:   <Added Later>
# Common System Loader
A Personal project for my Low Level Projects
All multi-byte fields are little-endian.

## Goals:
1. Prepare a minimal execution environment such that the kernel can begin executing C code immediately.
2. Load Kernel into Memory
3. Load Initrd if needed (LATER_GOAL)
4. Transfer control to the kernel.

## Non-Goals:
1. Anything not **EXPLICITLY** stated in Goals.

---

## What does the Kernel Expect?
### -> KASLR DOESNT EXIST (LATER_GOAL)

### How it looks:

CSL Boot Information Layout v1

| Offset | Size | Type     | Description           |
|--------|------|----------|-----------------------|
| 0x00   | 4    | uint32_t | CSL Boot Info Version |
| 0x04   | 8    | uint64_t | RAM Base              |
| 0x0C   | 8    | uint64_t | RAM Size              |

### Kernel Entry ABI

On entry to the kernel:

- x0 contains the address of the CSL Boot Information block.
- SP is initialized and 16-byte aligned.
- MMU is enabled.
- The kernel image has been fully relocated.
- .bss has been zero-initialized.
- .data has been initialized.
- The CPU is executing at EL1.
- Interrupts are disabled.


## Protocol Versions

### v1
- Boot info version
- RAM base
- RAM size