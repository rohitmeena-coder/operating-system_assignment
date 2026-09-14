Assignment 6 Submission — ROHIT MEENA, 2401MC17

Contents:
- diffs/        a6_Makefile.diff -- the only kernel-tree change (adds the 5
                new programs to UPROGS). All synchronization primitives are
                reused unmodified from Assignment 5 (sem_init/sem_wait/
                sem_post, shmget(), sleep(), uptime()) -- no new syscalls
                were needed for this assignment.
- tests/        bankers.c, deadlockdetect.c, ro_bad.c, ro_fixed.c,
                syncdeadlock.c
- logs/         captured serial-console output proving correct execution
                for Q1-Q4, including the genuine hang for the buggy Q3
                variant
- Assignment_6_write_up.pdf  concise write-up of the four solutions and
  results

Questions:
- Q1 Banker's Algorithm (bankers.c): safe sequence found for the initial
  state (P1,P3,P4,P0,P2); one request granted after the safety check, one
  correctly denied as unsafe and rolled back.
- Q2 Deadlock Detection via Resource-Allocation/Wait-For Graph
  (deadlockdetect.c): acyclic scenario correctly reports no deadlock;
  3-process circular-wait scenario correctly detects and prints the exact
  cycle P0 -> P1 -> P2 -> P0.
- Q3 Deadlock Prevention via Resource Ordering (ro_bad.c / ro_fixed.c):
  ro_bad.c reliably reproduces a genuine 2-process circular-wait deadlock
  as real forked xv6 processes (verified hang, see
  logs/q3_resourceorder_bad_HANGS.log); ro_fixed.c uses a consistent
  global lock-acquisition order and completes normally with identical
  timing.
  NOTE: filenames resourceorder_bad.c/resourceorder_fixed.c from the
  assignment brief were shortened to ro_bad.c/ro_fixed.c because xv6's
  file system limits names to 14 characters (DIRSIZ in fs.h) -- the
  original names collided on truncation and silently overwrote each
  other's directory entry in early testing.
- Q4 Combined Synchronization & Deadlock Avoidance (syncdeadlock.c): 5
  processes, 3 shared resource pools (2 printers, 1 scanner, 2 disks),
  each process needs 2 of the 3 pools per cycle. Strategy: resource
  ordering (reused from Q3) -- every process requests its lower-numbered
  resource type first, so a wait-for cycle can never form. All 5
  processes complete 4 cycles each with no resource pool ever
  over-subscribed and no process ever permanently blocked.

Build environment:
  apt-get install gcc-multilib g++-multilib qemu-system-x86 binutils
  (or on macOS: brew install i686-elf-gcc qemu)

Rebuild:
  Start from pristine xv6-public, apply Assignment 5's q1_peterson_*
  kernel/shared-memory/semaphore diffs first (sync.c is a new file, not a
  patch -- copy it in directly), then apply diffs/a6_Makefile.diff, copy
  in the tests/*.c files, and build:

  make TOOLPREFIX= clean
  make TOOLPREFIX=
  make TOOLPREFIX= fs.img
  make TOOLPREFIX= QEMU=qemu-system-i386 CPUS=1 qemu-nox

  Then at the xv6 shell prompt run: bankers | deadlockdetect | ro_bad |
  ro_fixed | syncdeadlock
