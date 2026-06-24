## OS Dev

This is a fun project for myself to learn more about how an operating system works and a lot C programming.

## Directory Structure

```
beanos/
├── Makefile                        # Top-level build orchestration
├── README.md
│
├── kernel/                         # Kernel binary — architecture-specific code
│   ├── Makefile
│   ├── arch/i386/
│   │   ├── boot.s                  # Multiboot entry, paging setup, higher-half jump
│   │   ├── crti.s / crtn.s         # C runtime init/fini stubs
│   │   ├── linker.ld               # Kernel memory layout
│   │   ├── tty.c                   # VGA text mode driver
│   │   └── vga.h                   # VGA color/entry helpers
│   ├── include/kernel/
│   │   ├── tty.h
│   │   ├── gdt.S                   # GDT flush / segment reload
│   │   ├── interrupt.S             # ISR stubs (exceptions 0-31)
│   │   ├── irq.S                   # IRQ stubs (IRQs 0-15)
│   │   └── paging.S                # loadPageDirectory / enablePaging
│   └── kernel/
│       └── kernel.c                # kernel_main() entry point
│
├── libc/                           # C library — builds libk.a (kernel) and libc.a (user)
│   ├── Makefile
│   ├── include/                    # Public headers (installed to sysroot)
│   │   ├── stdio.h
│   │   ├── stdlib.h
│   │   ├── string.h
│   │   ├── kernel.h                # Kernel-specific declarations
│   │   └── sys/
│   │       ├── cdefs.h
│   │       ├── gdt.h / idt.h / isr.h / irq.h
│   │       ├── io.h                # inb/outb/inw/outw
│   │       └── timer.h
│   ├── stdio/                      # printf, putchar, puts
│   ├── stdlib/                     # abort (future: malloc, free)
│   ├── string/                     # memcpy, memset, strlen, itoa, etc.
│   ├── sys/                        # GDT, IDT, ISR, IRQ, timer implementations
│   │   └── io/                     # Port I/O (inb, outb, inw, outw)
│   └── kernel/                     # Kernel-only modules (paging, PIC)
│
├── drivers/                        # [Phase 1+] Hardware drivers
│   ├── keyboard/                   # PS/2 keyboard (Phase 1)
│   ├── ata/                        # ATA/IDE disk (Phase 3)
│   ├── pci/                        # PCI bus enumeration (Phase 5)
│   └── net/                        # NIC driver — RTL8139 or E1000 (Phase 5)
│
├── fs/                             # [Phase 3] Filesystem layer
│   ├── vfs.c / vfs.h              # Virtual filesystem interface
│   └── fat16/                      # FAT16 implementation (or custom FS)
│
├── net/                            # [Phase 5] Network stack
│   ├── ethernet.c                  # Ethernet frame parsing
│   ├── arp.c                       # Address resolution
│   ├── ipv4.c                      # IPv4 layer
│   ├── icmp.c                      # Ping
│   ├── udp.c                       # UDP datagrams
│   └── tcp.c                       # TCP (stretch goal)
│
├── shell/                          # [Phase 1] Interactive shell
│   └── shell.c                     # Command loop, builtins, dispatcher
│
├── cc/                             # [Phase 6] Self-hosted C compiler
│   ├── lexer.c                     # Tokenizer
│   ├── parser.c                    # AST construction
│   ├── codegen.c                   # i386 code generation
│   └── cc.c                        # Driver (preprocessor, compile, assemble, link)
│
├── bin/                            # [Phase 4+] User-space programs (flat binaries)
│
└── sysroot/                        # Build output — mirrors installed system
    └── usr/
        ├── include/                # Installed headers
        ├── lib/                    # libk.a (kernel), libc.a (userspace)
        └── boot/
            └── beanos.kernel       # Final kernel binary
```

**Library split:**
- **libk.a** — libc compiled with `-ffreestanding` for use inside the kernel (no syscalls, direct hardware access)
- **libc.a** — [Phase 4] libc compiled for user-space programs (calls kernel via syscalls)
- **drivers/, fs/, net/** — linked directly into the kernel binary (monolithic kernel)

---

## Development Plan

### Currently Working
- Higher-half kernel (mapped at 0xC0000000)
- GDT, IDT, ISRs (exceptions 0-31)
- PIC initialization and IRQ handling (IRQs 0-15)
- PIT timer (50 Hz)
- VGA text mode driver (80x25, colors, scrolling, cursor)
- Basic libc (printf, string functions, I/O ports)

---

### Phase 1: Keyboard Input & Interactive Shell - DONE

**1.1 — PS/2 Keyboard Driver**
[x] Register IRQ1 handler for keyboard interrupts
[x] Read scancodes from port 0x60
[x] Scancode set 1 translation table (make/break codes)
[x] Handle modifier keys (shift, ctrl, alt, capslock)
[x] Expose `keyboard_getchar()` or a ring buffer for input

**1.2 — Line Input Buffer**
[x] Implement a `getline()` / `readline()` function
[x] Keyboard buffer with backspace support
[x] Echo typed characters to screen
[x] Return completed line on Enter

**1.3 — Basic Shell**
[x] Simple command loop: print prompt, read line, parse command, execute
[x] Built-in commands to start with: `help`, `clear`, `echo`, `tick` (show timer), `reboot`
[x] Command dispatcher (string compare against known commands)

---

### Pre-Phase 2: Blockers to Clear First - DONE
Plumbing and bug fixes that Phase 2 depends on. The first two are hard blockers —
without them there is no memory map to parse. The rest are debugging quality-of-life
that you will badly want the moment you start chasing allocator bugs.

**P2.0 — Pass the multiboot info into the kernel (hard blocker)**
- [x] On entry to `_start`, the bootloader leaves the multiboot magic in `EAX` and a
      pointer to the multiboot info struct in `EBX`. `boot.s` currently calls
      `kernel_main` with no arguments and discards both.
- [x] Push `EBX` (info pointer) and `EAX` (magic) as arguments before `call kernel_main`.
      The paging setup uses `ecx/edx/esi/edi` and does not clobber `ebx`, so the pointer
      survives to the call site.
- [x] Change `kernel_main` signature to `(uint32_t magic, multiboot_info_t* mbi)` and
      verify the magic before trusting the pointer.
- [x] Add a `multiboot.h` defining the info struct and memory-map entry layout.

**P2.1 — Make the multiboot info reachable after the identity unmap (hard blocker)**
- [x] The info struct lives at a low physical address (typically under 1 MiB), but
      `boot.s` unmaps the identity mapping right before entering `kernel_main`. The
      higher-half mapping only covers `_kernel_start.._kernel_end`, so dereferencing the
      physical pointer afterward page-faults.
- [x] Pick one: parse/copy the memory map *before* unmapping identity, **or** map the
      multiboot info page into the higher half and access it at `phys + 0xC0000000`.

**P2.2 — printf hardening for memory debugging**
- [x] Add `%u` (unsigned decimal) and `%p` (pointer) — Phase 2 prints addresses and
      pointers constantly. (`%x` already handles high addresses like `0xC0000000`.)
- [x] Fix `written += 1` → `written += len` in the `%d`/`%x` branches (wrong return count).
- [x] Decide on signed handling: `itoa` takes `uint32_t` and never emits `-`, so `%d` of
      a negative prints wrong.

**P2.3 — Minor polish (optional, not blocking)**
- [x] Backspace: `terminal_putchar('\b')` re-increments the column and never calls
      `update_cursor`, so the visible cursor lags the erase.
- [x] `strtok`: trailing-separator edge case can emit an empty token / off-by-one count.

---

### File Placement & Include Conventions
How includes resolve here (set by `kernel/Makefile`: `-Iinclude` plus the sysroot `-isystem`).
Follow these for every new file in Phases 2–5 and includes "just work" with no Makefile fiddling.

- **Kernel headers → `kernel/include/kernel/<name>.h`**, included as `#include <kernel/<name>.h>`.
  This is the default for almost everything below. The dir is on the include path and is
  auto-copied into the sysroot by `make install-headers`, so any `.c` can include it.
- **Standard C library headers → `libc/include/<name>.h`**, included as `#include <name.h>`
  (e.g. `stdlib.h`, `string.h`, `stdio.h`). Only for genuine libc APIs (`malloc`, `memcpy`, …).
- **Low-level x86 helpers → `libc/include/sys/<name>.h`**, included as `#include <sys/<name>.h>`
  (e.g. `sys/io.h` for `inb`/`outb`).
- **Private header used by only one sibling `.c` → keep it next to the `.c`** and use
  `#include "name.h"` (quotes search the file's own directory first). This is how
  `arch/i386/multiboot.h` works — do not use quote-includes across directories.

For each new `.c`: add its object to `KERNEL_OBJS` in `kernel/Makefile`, and place the source
by kind — **arch-specific** (touches x86 registers/ports/paging structures) in
`kernel/arch/i386/`; **arch-independent kernel logic** in `kernel/mm/`, `kernel/drivers/`,
`kernel/fs/`. (These consolidate the aspirational top-level `drivers/`/`fs/`/`net/` under
`kernel/` so the existing build and `-Iinclude` cover them with zero extra wiring. It's a
monolithic kernel — it all links into one binary regardless.)

---

### Phase 2: Memory Management
Required before you can build anything dynamic (buffers, filesystem caches, process stacks).

**2.1 — Physical Memory Manager**
- Parse multiboot memory map to find available RAM
- Bitmap or stack-based frame allocator
- `alloc_frame()` / `free_frame()` for 4K physical pages
- **Files:**
  - `kernel/mm/pmm.c` — allocator; the bitmap is a `static` array inside it (no header needed)
  - `kernel/include/kernel/pmm.h` — `pmm_init()`, `alloc_frame()`, `free_frame()` → `<kernel/pmm.h>`
  - reads regions from `<kernel/boot.h>` (`g_boot_info`); reserves the kernel's own range using
    the `_kernel_start`/`_kernel_end` symbols from `kernel/arch/i386/linker.ld`

**2.2 — Kernel Heap (malloc/free)**
- Simple allocator on top of the frame allocator (e.g. linked-list first-fit or dlmalloc-style)
- Implement `kmalloc()`, `kfree()`, `krealloc()`
- Add `malloc`/`free` to libc
- **Files:**
  - `kernel/mm/heap.c` — `kmalloc`/`kfree`/`krealloc` (+ optional early bump allocator)
  - `kernel/include/kernel/heap.h` — prototypes → `<kernel/heap.h>`
  - libc bridge (optional): `libc/stdlib/malloc.c` forwarding `malloc`/`free` → `kmalloc`/`kfree`
    under `#ifdef __is_libk`; declarations go in the existing `libc/include/stdlib.h`

**2.3 — Virtual Memory Manager**
- Page directory/table management functions
- `map_page()` / `unmap_page()` for kernel and (later) user space
- Page fault handler that can grow the heap or report errors
- **Files:**
  - `kernel/mm/vmm.c` — `map_page`/`unmap_page` + recursive-mapping setup
  - `kernel/include/kernel/vmm.h` — prototypes → `<kernel/vmm.h>`
  - page-fault handler (vector 14): register the C handler via the existing ISR dispatch
    (`register_interrupt_handler` in `libc/sys/isr.c`; stubs in `kernel/include/kernel/interrupt.S`)
    — the handler body can live in `vmm.c`

---

### Phase 3: Storage & Filesystem
Persistent storage so the OS can actually do something useful.

**3.1 — ATA/IDE Disk Driver**
- PIO-mode ATA driver (ports 0x1F0-0x1F7)
- `ata_read_sector()` / `ata_write_sector()` for 512-byte sectors
- Identify drive command to detect attached disks
- **Files:**
  - `kernel/drivers/ata/ata.c` — polling PIO driver; uses `<sys/io.h>` for port access
  - `kernel/include/kernel/ata.h` — `ata_identify()`, `ata_read_sector()`, `ata_write_sector()` → `<kernel/ata.h>`
  - build/test: add a `disk.img` (FAT16, made with `mkfs.fat`/`mtools`) and attach it in the
    `run`/`debug` targets via `-drive file=disk.img,format=raw,if=ide`

**3.2 — Simple Filesystem (FAT16 or custom)**
- Start with a simple flat filesystem or FAT16 (well-documented, easy to create images)
- Core operations: `open`, `read`, `write`, `close`, `readdir`
- VFS (Virtual Filesystem) layer so you can swap implementations later
- Mount a disk image at boot
- **Files:**
  - `kernel/fs/vfs.c` + `kernel/include/kernel/vfs.h` — the node/ops abstraction → `<kernel/vfs.h>` (build this first)
  - `kernel/fs/fat16/fat16.c` + `kernel/include/kernel/fat16.h` — FAT16 backend behind the VFS → `<kernel/fat16.h>`
  - depends on `<kernel/ata.h>` (disk I/O) and `<kernel/heap.h>` (buffers / sector cache)

**3.3 — Shell Filesystem Commands**
- `ls` — list files
- `cat` — print file contents
- `write` — write text to a file
- `mkdir` — create directory (if using a hierarchical FS)
- `rm` — delete file
- **Files:**
  - extend the existing `kernel/kernel/shell.c` — add `ls`/`cat`/`write`/`mkdir`/`rm` rows to the
    command table; each handler calls into `<kernel/vfs.h>` (no new module needed)

---

### Phase 4: Process Management
Multitasking and user-space programs.

**4.1 — Task Switching**
- Task state structure (registers, page directory, kernel stack)
- Timer-based preemptive context switching
- Kernel threads first (same address space, simpler)

**4.2 — User Mode**
- TSS (Task State Segment) setup for ring 0 ↔ ring 3 transitions
- System call interface (int 0x80 or sysenter)
- Load and execute simple flat binaries in user space

**4.3 — Basic Syscalls**
- `write()`, `read()`, `exit()`, `exec()`, `open()`, `close()`
- Wire syscalls through the IDT to kernel handlers

---

### Phase 5: Networking
The final stretch — getting packets in and out.

**5.1 — Network Card Driver**
- RTL8139 or E1000 (both well-documented, QEMU supports both)
- PCI bus enumeration to find the NIC
- DMA buffer setup, interrupt-driven packet receive/transmit

**5.2 — Network Stack**
- Ethernet frame parsing (src/dst MAC, ethertype)
- ARP (address resolution)
- IPv4 (header parsing, basic routing for single subnet)
- ICMP (ping support — great first milestone)
- UDP (simple datagram send/receive)
- TCP (connection state machine — stretch goal)

**5.3 — Network Shell Commands**
- `ping` — send ICMP echo requests
- `netinfo` — show IP/MAC configuration
- `send` / `recv` — basic UDP messaging

---

### Phase 6: Self-Hosted C Compiler
The ultimate goal — the OS can compile itself.

**6.1 — Minimal C Compiler (cross-compiled first)**
- Subset of C: functions, variables, if/else/while/for, pointers, arrays, structs
- Lexer: tokenize source into keywords, identifiers, literals, operators
- Parser: recursive descent producing an AST
- Code generator: emit i386 assembly (AT&T syntax) from the AST
- Target output: assembly files that can be assembled by a bundled assembler

**6.2 — Assembler & Linker**
- Simple i386 assembler: translate AT&T assembly → flat binary or ELF relocatable objects
- Minimal linker: resolve symbols, combine sections, produce executables
- Alternative: port an existing small assembler (e.g. fasm-style) if writing one from scratch is too costly

**6.3 — Preprocessor & Driver**
- `#include`, `#define`, `#ifdef` / `#endif`, `#pragma once`
- Compiler driver (`cc`) that chains: preprocess → compile → assemble → link
- Support compiling multi-file programs

**6.4 — Self-Hosting Bootstrap**
- Get the compiler to compile itself on beanos (reading source from the filesystem, writing output to disk)
- Verify: compiler compiled by itself produces identical output to the cross-compiled version
- Stretch: compile the kernel itself from within beanos

**Compiler scope (what to support):**
- Types: `char`, `int`, `short`, `long`, `void`, pointers, arrays, structs, enums, typedefs
- Control flow: `if`/`else`, `while`, `for`, `do`/`while`, `switch`/`case`, `goto`
- Operators: arithmetic, bitwise, logical, comparison, assignment, address-of/dereference
- Functions: declarations, definitions, variadic (`...`) support
- Preprocessor: `#include`, `#define` (object and function-like), `#if`/`#ifdef`/`#endif`
- No need for: floating point, C99/C11 features, full standard library, optimization passes

