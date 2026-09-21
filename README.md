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

### Phase 2: Memory Management - DONE
Required before you can build anything dynamic (buffers, filesystem caches, process stacks).

**2.1 — Physical Memory Manager**
[x] Parse multiboot memory map to find available RAM
[x] Bitmap or stack-based frame allocator
[x] `alloc_frame()` / `free_frame()` for 4K physical pages

**2.2 — Kernel Heap (malloc/free)**
[x] Simple allocator on top of the frame allocator (e.g. linked-list first-fit or dlmalloc-style)
[x] Implement `kmalloc()`, `kfree()`, `krealloc()`
[x] Add `malloc`/`free` to libc

**2.3 — Virtual Memory Manager**
[x] Page directory/table management functions
[x] `map_page()` / `unmap_page()` for kernel and (later) user space
[x] Page fault handler that can grow the heap or report errors

---

### Phase 3: Storage & Filesystem
Persistent storage so the OS can actually do something useful.

**3.1 — ATA/IDE Disk Driver**
[x] PIO-mode ATA driver (ports 0x1F0-0x1F7)
[x] `ata_read_sector()` / `ata_write_sector()` for 512-byte sectors
[x] Identify drive command to detect attached disks

**3.2 — Simple Filesystem (FAT16 or custom)**
[x] Start with a simple flat filesystem or FAT16 (well-documented, easy to create images)
[x] Core operations: `open`, `read`, `write`, `close`, `readdir`
[x] VFS (Virtual Filesystem) layer so you can swap implementations later
[x] Mount a disk image at boot

**3.3 — Shell Filesystem Commands**
[x] `ls` — list files
[x] `cat` — print file contents
[x] `write` — write text to a file
[x] `mkdir` — create directory (if using a hierarchical FS)
[x] `rm` — delete file

**3.4 — Hierarchical Filesystem (subdirectories & paths)**
Everything through 3.3 is flat / root-only. This makes FAT16 properly hierarchical.
Build in dependency order — the cluster-aware walk underpins everything else.

[ ] Cluster-aware directory walk — `readdir`/`finddir`/`create` must handle **both** the root
    (fixed region at `fs.root_start`) **and** subdirectories (cluster chains from the directory's
    start cluster). Factor the root-only walk into one "iterate a directory's entries" helper that
    branches root-vs-chain internally, so the three functions stop hardcoding the root region.
[ ] `mkdir` — allocate a cluster, initialize it with `.` (self) and `..` (parent) entries, zero the
    rest of the cluster, then add a directory entry (attr `0x10`) in the parent. The `.`/`..` links
    are the new concept — every directory but the root starts with self + parent.
[ ] Path resolution — split a path on `/` and resolve component-by-component via `finddir`, starting
    from the root (absolute path) or the cwd (relative). Fail if a component is missing or isn't a
    directory.
[ ] Current working directory — the shell tracks a `cwd` node (plus a path string for the prompt);
    `.`/`..` resolve through the entries `mkdir` writes.
[ ] `cd <path>` command, and make `ls`/`cat`/`write`/`rm`/`mkdir` accept a **path** (resolve it first)
    instead of a bare root-level name.
- **Files:**
  - `kernel/fs/fat16/fat16.c` — cluster-aware directory walk (root vs. chain); `fat16_mkdir`
    (or `create` with `VFS_DIRECTORY`: `.`/`..` init + parent entry)
  - `kernel/fs/vfs.c` + `<kernel/vfs.h>` — a path resolver (e.g. `vfs_resolve(path)` walking
    components via `finddir`); optional `vfs_mkdir` dispatcher
  - `kernel/kernel/shell.c` — `cd` command, a `cwd` node, and path-aware `ls`/`cat`/`write`/`rm`/`mkdir`

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

