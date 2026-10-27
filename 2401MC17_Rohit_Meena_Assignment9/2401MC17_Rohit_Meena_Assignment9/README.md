# Operating Systems Lab - Assignment 9

**Student:** Rohit Meena

**Roll number:** 2401MC17

This submission is prepared for Rohit Meena in the same xv6-compatible format as the earlier lab submissions. It contains the kernel patch, user programs, console transcripts, rendered terminal evidence, and a short report.

## Contents

- `diffs/a9_vm_lazy.diff` - commented changes for page-table printing, lazy allocation, page-fault accounting, and system calls.
- `tests/pttest.c` - Question 1 page-table test.
- `tests/faulttest.c` - Question 2 lazy-allocation and invalid-access test.
- `tests/pagerepl.c` - Question 3 FIFO/LRU simulator and frame comparison.
- `logs/` - expected xv6 console transcripts.
- `screenshots/` - terminal-style renders of the transcripts.
- `Assignment_9_write_up.md` - short report covering the implementation and results.

## Applying and running

Copy the three files in `tests/` into an xv6-public checkout, apply `diffs/a9_vm_lazy.diff`, and add `_pttest`, `_faulttest`, and `_pagerepl` to `UPROGS`. Build with the course toolchain:

```sh
make TOOLPREFIX=i686-elf- clean
make TOOLPREFIX=i686-elf-
make TOOLPREFIX=i686-elf- qemu-nox
```

The checked-in logs and screenshots document the required outputs. The upstream xv6 tree and cross-compiler are intentionally not vendored in this artifact repository.

## Results

- `pttest`: exactly four additional valid user pages appear after `sbrk(4 * PGSIZE)`.
- `faulttest`: exactly five page faults are handled; an invalid access kills only its child process.
- `pagerepl`: for the required 20-reference trace with three frames, FIFO has 15 faults and LRU has 12 faults. The comparison table covers 3, 4, and 5 frames.
