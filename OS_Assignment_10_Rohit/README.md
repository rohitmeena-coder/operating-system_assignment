# Operating Systems — Assignment 10 (xv6)

Complete, working solution. All code was compiled into xv6 and actually run
in QEMU; the outputs in `outputs/` are from that real run.

## Contents
- `kernel/` — modified xv6 kernel sources (every change is commented):
  - `kalloc.c` — Q1: allocation/free counters + `memstats()` implementation
  - `vm.c` — Q2/Q3: `getpte()` (walkpgdir wrapper) and `setreadonly_vm()`
  - `sysproc.c` — handlers: `sys_memstats`, `sys_vtop`, `sys_pageinfo`, `sys_setreadonly`
  - `syscall.c`, `syscall.h` — new syscall numbers 22–25 and dispatch table
  - `defs.h`, `user.h`, `usys.S` — prototypes and user-space stubs
  - `Makefile` — the four test programs added to UPROGS
- `user_programs/` — `memtest.c` (Q1), `vttest.c` (Q2), `pagetest.c` (Q3),
  `pagereplace.c` (Q4: FIFO, LRU, Optimal + comparison table)
- `outputs/` — terminal output for each question as `.png` screenshots and
  the same text as `.txt` files
- `report/` — `Report.pdf` (2 pages) and its source `report.md`

## How to build and run
1. Copy `kernel/*` and `user_programs/*` into a fresh xv6 (xv6-public) tree,
   overwriting the originals.
2. Build: `make` and `make fs.img`
   (On a very new GCC, stock xv6 may need two stock-code warnings demoted:
   `make CC='gcc -Wno-error=array-bounds -Wno-error=infinite-recursion'` —
   this only affects unmodified xv6 files `mp.c`/`sh.c`, not this code.)
3. Run: `make qemu-nox`, then at the `$` prompt run:
   `memtest`, `vttest`, `pagetest`, `pagereplace`

## Results summary
- Q1: allocated pages 211 → 215 after `sbrk` of 4 pages → back to 211 after
  release; free pages 56789 → 56785 → 56789.
- Q2: virtual addresses translate to physical frame + offset; unmapped
  addresses return -1.
- Q3: page flags go from P=1 W=1 U=1 to W=0 after `setreadonly`; the next
  write raises trap 14 (page fault) and xv6 kills the process.
- Q4: page faults — 3 frames: FIFO 15, LRU 12, Optimal 9;
  4 frames: 10 / 8 / 8; 5 frames: 9 / 7 / 7. Optimal is best, LRU is the
  best practical algorithm, FIFO is worst for this reference string.
