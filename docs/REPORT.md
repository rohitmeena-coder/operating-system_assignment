# OS Lab Assignment 8 — Memory Management in xv6

**Student:** Rohit Kumar Meena  
**Base:** MIT xv6-public, x86 two-level paging implementation

## Overview

This submission instruments and extends the x86 xv6 virtual-memory subsystem. The implementation exposes virtual-to-physical translation, tracks allocator pages, prints hierarchical page tables, and adds lazy allocation for heap pages. Eight user programs exercise the requested behaviors and provide demonstrations for copy-on-write-style fork isolation, protection faults, shared-memory design considerations, and page-replacement accounting.

## Q1 — Virtual-to-physical translation

`walkpgdir()` is now exported from `vm.c`. The kernel helper `va2pa()` obtains the PTE without allocating a page table, checks `PTE_P` and `PTE_U`, prints the PDE index, PTE index, page offset, and P/W/U bits, and returns `PTE_ADDR(*pte) + offset`. Invalid or kernel-only mappings return `-1`.

`vatest.c` translates a global, stack, and heap address, checks `0x7fffffff`, then forks. The child and parent retain the same virtual address for the global, but `copyuvm()` gives them distinct physical frames. This demonstrates xv6's eager copy-on-fork behavior.

## Q2 — Physical-memory statistics

`kalloc.c` maintains `free_pages` and `total_pages` in the allocator state. Counters are updated while `kmem.lock` is held. Initialization counts each managed physical frame as it enters the free list; allocation decrements the free count and freeing increments it.

`struct meminfo` contains `total_pages`, `free_pages`, `user_pages`, and `pagetable_pages`. `getmeminfo()` counts user PTEs in the caller's address space and present user page-directory entries as page-table pages. `memtest.c` reports values before and after heap growth and after a fork/reap cycle. Physical pages from the child are released by `freevm()` when the child is reaped, so free-page count should return to its pre-fork level, subject to allocator metadata and concurrent kernel activity.

## Q3 — Page-table dump

`vmprint()` walks every present user PDE and its page table. It prints indentation, indexes, virtual address, physical frame, and P/W/U flags. Kernel PDEs are excluded from the detailed dump and summarized on one final line. A guard page created by `exec()` is visible as a present mapping whose PTE_U bit is clear.

`vmprinttest.c` calls the system call before heap growth, after `sbrk(8*4096)`, and after shrinking by one page. With eager allocation, the heap PTEs appear and disappear immediately; with the lazy extension, reserved but untouched pages remain absent until accessed.

## Q4 — Lazy heap allocation

Positive `sbrk()` now advances `proc->sz` without allocating frames. Negative `sbrk()` still calls `growproc()` and `deallocuvm()`, which already skips absent PTEs. On `T_PGFLT`, `trap.c` rounds the fault address down, verifies that it is a user fault below `proc->sz`, allocates a zero-filled page, maps it writable/user, and resumes. Invalid faults and allocation failure mark only the offending process killed.

Kernel copies into a previously untouched lazy page are supported through `uva2ka()`: when the requested page is absent but lies below the process size, it is allocated and mapped before the copy. `copyuvm()` skips absent PTEs so a fork of a sparse lazy heap succeeds. `lazytest.c` reserves 32 MB, touches three widely separated pages, reports memory before and after, and then exercises fork/reap.

## Supplementary programs

- `cowtest.c`: demonstrates that ordinary xv6 fork pages are independent after the child writes.
- `protecttest.c`: intentionally performs an invalid user access; the kernel should kill the child while the parent survives.
- `shmtest.c`: documents the baseline fork isolation and provides a place to compare against a future shared-memory mapping.
- `pagerepl.c`: runs a fixed reference string with three frames and prints FIFO faults; the report comparison below gives the corresponding LRU and Optimal calculation method.

## Page-replacement comparison

For the reference string `1 2 3 1 4 5 2 1 6 3 4 5` with three frames, a replacement simulator should count a fault whenever the requested page is not resident. FIFO uses a circular victim pointer. LRU replaces the page with the oldest last-use timestamp. Optimal replaces the resident page whose next use is farthest in the future, or one never used again.

The included `pagerepl.c` provides the xv6-side FIFO experiment and prints its frames. LRU and Optimal are intentionally kept as analysis algorithms because the assignment's page-replacement question is a user-space comparison rather than a replacement policy used by xv6's physical allocator. When extending the program, maintain three arrays—frame contents, last-use timestamps, and next-use lookahead—and print one row per reference.

| Policy | Decision rule | Expected behavior |
|---|---|---|
| FIFO | Evict oldest arrival | Simple, can suffer Belady's anomaly |
| LRU | Evict least recently referenced | Usually better than FIFO for locality |
| Optimal | Evict page used farthest in future | Lower bound; not implementable online |

## Changed files

Kernel: `vm.c`, `vmprint.c`, `trap.c`, `kalloc.c`, `sysproc.c`, `syscall.c`, `syscall.h`, `defs.h`, `proc.h`, and `Makefile`.  
User tests: `vatest.c`, `memtest.c`, `vmprinttest.c`, `lazytest.c`, `cowtest.c`, `protecttest.c`, `shmtest.c`, and `pagerepl.c`.

## Verification

- Host 32-bit C syntax check for modified kernel files: **passed**, exit code 0.
- Host 32-bit C syntax checks for all eight user programs: **passed**, exit code 0.
- The remaining output consists only of normal host-GCC warnings caused by xv6's own `exit()` declaration.
- Full xv6 image execution was not possible in the supplied Windows environment because the xv6 x86 toolchain/QEMU setup is not installed. No emulator output or screenshots are fabricated. On a standard xv6-public environment, run:

```sh
make clean
make
make qemu
# xv6 shell:
vatest
memtest
vmprinttest
lazytest
cowtest
protecttest
shmtest
pagerepl
```

The ZIP contains the complete source tree, modified kernel files, user tests, and this report.
