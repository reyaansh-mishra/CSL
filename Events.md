[11/07/2026 @ 6:56PM]: First Double-Include-Double-Define Error:

```console
 *  Executing task: ./build.sh 

[AS] src/boot.s
[CC] src/main.c
[CC] src/utils/print.c
[LD] csl.elf
ld.lld: error: duplicate symbol: uart_ptr
>>> defined at main.c
>>>            build/main.o:(uart_ptr)
>>> defined at print.c
>>>            build/utils/print.o:(.data+0x0)
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

Fix: Use #pragma once

---

[16/07/2026 @ 10:07PM]: Switched to C++

---

[20/07/2026 @ 6:25PM]: Added Support for AMD64 Platforms. AARCH64 IS STILL THE MAIN TARGET.

---

[29/07/2026 @ 8:25PM]: MMU, FINALLY!
commit fbbd2b09631db72aa5a53a6e3a0014f36d95d99d (HEAD -> main, origin/main, origin/HEAD)
Author: Reyaansh Mishra <reyaanshmishr2@gmail.com>
Date:   Wed Jul 29 20:28:03 2026 +0530

    MMU MIGHT JUST ACTUALLY BE WORKING

 build.sh                                 |    5 +-
 csl.efi                                  |  Bin 14336 -> 15360 bytes
 csl.pdb                                  |  Bin 135168 -> 139264 bytes
 esp/EFI/BOOT/BOOTAA64.efi                |  Bin 14336 -> 15360 bytes
 includes/CSL/specific-includes/arm64.hpp |    2 +-
 includes/CSL/specific-includes/mmu.hpp   |    4 +-
 qemu.log                                 | 5675 ++++++++++++++++++++++++++++++++++++++++++++++++++---------------------------------------------------------------------------------------------------------------------------------------
 qemu.sh                                  |    5 +-
 src/arch/arm64/interrupts.s              |    8 +-
 src/arch/arm64/mmu_lowlevel.s            |   14 +-
 src/arch/arm64/vectors.s                 |   89 +--
 src/boot/main.cpp                        |    4 +-
 src/mmu/descriptors/l1_l2.cpp            |    1 +
 src/mmu/translation_tables.cpp           |  107 +++-
 src/terminal/terminal.cpp                |   16 +-
 15 files changed, 1699 insertions(+), 4231 deletions(-)
:

---

[31/07/2026 @ 12:23PM]: Payload can now dicate where in virtual addr it wants to be.

---

[06/08/2026 @ 8:41PM]: CSL near v1. Payload can now physically get itself remapped.
Boot Flow:

```
UEFI Entry
    |
    v
Initialize MemMappr
    |
    v
Bootstrappr
    |
    v
Build translation tables
    |
    v
Mask interrupts + install vectors
    |
    v
ExitBootServices
    |
    v
Load private TTBR
    |
    v
Enable MMU
    |
    v
Relocate CSL
    |
    v
Jump payload
```