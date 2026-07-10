# Common System Loader
A Personal project for my Low Level Projects

## Goals:
1. Prepare a minimal execution environment such that the kernel can begin executing C code immediately.
2. Load Kernel into Memory
3. Load Initrd if needed (LATER_GOAL)
4. Execute Kernel

## Non-Goals:
1. Anything not **EXPLICITLY** stated in Goals.

---

## What does the Kernel Expect?
### -> KASLR DOESNT EXIST (LATER_GOAL)

### How it looks:

CSL Boot Information Layout v1

| Offset | Size | Description            |
| ------ | ---- | -----------------------|
| 0x00   | 4    | CSL Boot Info Version  |
| 0x04   | 8    | RAM Base               |
| 0x0C   | 8    | RAM Size               |