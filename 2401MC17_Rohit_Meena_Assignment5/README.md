Assignment 5 Submission — ROHIT MEENA, 2401MC17

Contents:
- diffs/        per-question patches against pristine xv6-public
- tests/        peterson.c, prodcons.c, readwrite.c, dining.c
- screenshots/  terminal output proving correct execution for Q1–Q4
- Assignment_5_write_up.pdf  concise write-up of the four solutions and results

Questions:
- Q1 Peterson's algorithm: final counter 20, mutual-exclusion violations 0
- Q2 Producer-consumer: 20 items produced and consumed in FIFO order
- Q3 Readers-writers: final shared value 8, synchronization violations 0
- Q4 Dining philosophers: all five philosophers completed 5 cycles without deadlock

Build environment:
  brew install i686-elf-gcc qemu

Rebuild:
  Apply the q1_peterson_* kernel/shared-memory/semaphore diffs first, then add
  the relevant question test and build xv6 with:

  make TOOLPREFIX=i686-elf- clean
  make TOOLPREFIX=i686-elf-
  make TOOLPREFIX=i686-elf- fs.img
  make TOOLPREFIX=i686-elf- QEMU=qemu-system-i386 CPUS=1 qemu-nox
