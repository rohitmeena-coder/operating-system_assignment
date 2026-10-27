# Assignment 9 Write-up

**Student:** Rohit Meena  
**Roll number:** 2401MC17

## Q1 - Printing a process page table

Rohit's `vmprint()` walks the current process's two-level x86 page table with `walkpgdir()`. It ignores PDEs at or above `KERNBASE`, prints each valid user virtual/physical mapping with P/W/U flags, and reports both the number of valid user pages and the number of page-table pages. `pttest` calls it before and after `sbrk(4 * PGSIZE)`. The valid-page count increases by exactly four.

## Q2 - Lazy allocation and page faults

Positive `sbrk()` now grows `proc->sz` without allocating frames. `deallocuvm()` and `copyuvm()` skip absent PTEs. The `T_PGFLT` handler validates that the fault lies in the process heap, allocates and zeroes one page, maps it writable/user-accessible, and increments `proc->pagefaults`. `getfaults()` exposes the count. `faulttest` touches five distinct pages and reports five handled faults; a child that accesses above its allocation is killed while the parent continues.

## Q3 - FIFO and LRU page replacement

`pagerepl` accepts a frame count and optional reference string, prints the frame state and hit/fault for every reference, and then runs the required trace for 3, 4, and 5 frames. FIFO evicts the oldest loaded page. LRU evicts the page with the oldest recent-use timestamp. For three frames the required result is FIFO = 15 faults and LRU = 12 faults. The same trace gives 10/8 faults for four frames and 9/7 faults for five frames.

## Submission checklist

This Rohit Meena submission includes the modified kernel files in one clearly commented diff, `pttest.c`, `faulttest.c`, and `pagerepl.c`, one terminal transcript and screenshot for each question, and this short report.
