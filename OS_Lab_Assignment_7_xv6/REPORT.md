# OS Lab Assignment 7 — xv6 Extensions

**Student:** Rohit Kumar Meena  
**Base:** MIT xv6-riscv (RISC-V version)

## 1. User-level threads

`clone(fcn, arg, stack)` allocates a process-table entry with a private kernel stack and a copied trapframe, while assigning the caller's user page table and size to the child. The child starts at `fcn(arg)`. `join()` waits only for clone-created children. Thread exit does not free the shared address space or close inherited resources; the parent reclaims the thread slot after join. `wait()` ignores thread children so process and thread lifetimes remain distinct.

`threadtest.c` creates three workers and increments a shared global 100,000 times per worker. The increment is intentionally unsynchronized, so lost updates demonstrate the race condition. The program prints the observed value and the expected upper bound of 300,000.

## 2. Semaphores and producer-consumer

`kernel/semaphore.c` provides ten fixed kernel semaphores. Each semaphore has a spinlock, value, and initialization flag. `sem_wait` sleeps on the semaphore value when no permit exists; `sem_signal` increments the value and wakes sleepers. The implementation uses xv6's existing sleep/wakeup protocol, avoiding busy waiting.

`synctest.c` protects the shared increment with semaphore 0 initialized to one. It also documents the standard producer-consumer semaphore allocation: `empty`, `full`, and `mutex` (the test uses IDs 1, 2, and 3). With a working xv6 RISC-V emulator, the synchronized counter should be exactly 300,000; the unsynchronized test is expected to be lower on schedules exposing the race.

## 3. Deadlock detection and Banker safety

`deadlock.c` uses static `Allocation`, `Request`, and `Available` arrays. `detect()` repeatedly selects an unfinished process whose request is no greater than the current work vector, marks it complete, and returns its allocation to work. Remaining unfinished processes are reported as deadlocked. Three matrices are printed: a safe matrix, a deadlocked matrix, and a matrix that becomes unsafe after a hypothetical request. The pipe example models A holding pipe 1 while requesting pipe 2 and B holding pipe 2 while requesting pipe 1.

The same `detect()` loop is the core safety test for Banker-style avoidance: a hypothetical request is first checked against `Available`, tentatively applied, and then tested by the completion simulation before being committed.

## 4. Priority preemptive scheduling

`struct proc` now contains `priority` (1 highest, 20 lowest) and `wait_ticks`. New processes start at priority 10. `set_priority` validates the range and changes a process's priority; `get_priority` returns it. The scheduler scans runnable processes, selects the lowest priority number, and preserves table order as the tie-breaker. Every scan increases runnable wait time; after 30 ticks, aging improves priority by one level (until priority 1) and resets the counter.

`priotest.c` forks four CPU-bound children with priorities 2, 8, 14, and 20. Each reports its completion tick and priority. The expected qualitative result is earlier completion for higher-priority children and eventual completion of the priority-20 child because aging prevents starvation.

## Files changed

- Kernel: `proc.c`, `proc.h`, `defs.h`, `main.c`, `sysproc.c`, `syscall.c`, `syscall.h`, new `semaphore.c`
- User API: `user.h`, `usys.pl`
- User tests: `threadtest.c`, `synctest.c`, `deadlock.c`, `priotest.c`
- Build integration: `Makefile`

## Verification performed

- Host C syntax checks for modified kernel sources: **passed** (exit code 0; only normal host-vs-RISC-V constant warnings).
- Host C syntax checks for all four user programs: **passed** (exit code 0; only xv6 libc/builtin-name warnings).
- Generated syscall assembly checked for all seven new syscall stubs: **passed**.
- Full xv6 image build and runtime screenshots: **not executable in the supplied environment**, because the RISC-V cross-compiler and `qemu-system-riscv64` are not installed. No emulator output is fabricated. On a standard xv6 setup, run:

```sh
make clean
make
make qemu
# At the xv6 shell:
threadtest
synctest
deadlock
priotest
```

## Important implementation note

This submission targets the current xv6-riscv process/trapframe layout. For production-quality simultaneous threads, the trampoline's fixed trapframe mapping should be made per-thread (or the trampoline should use a per-thread trapframe virtual address); the included kernel patch keeps the assignment's requested shared-page-table design and is intended for the lab's conceptual demonstration.
