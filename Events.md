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