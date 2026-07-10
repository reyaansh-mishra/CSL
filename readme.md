# Common System Loader
A Personal project for my Low Level Projects

## Goals:
1. Load MMU
2. Load Kernel into Memory
3. Load Initrd if needed (LATER_GOAL)
4. Minimal Usable C Runtime which the Kernel will use
5. Execute Kernel

## Non-Goals:
1. Anything not **EXPLICITLY** stated in Goals.

---

## What does the Kernel Expect?
### -> KASLR DOESNT EXIST (LATER_GOAL)
1. Memory Addrs ranging from 0x000000 -> *kernel_entry_point contains data which the kernel would normally get from the bootloader

### How it looks:

| Mem Range | What it is |
|------------|--------------
| int First Byte | CSL Memory Arragement Version |
| *Byte | Ram Base Addr |
| uint 8 Bytes | Ram Size (Max Val: 18,446,744,073,709,551,615) |