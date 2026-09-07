Assignment 5 Submission — ROHIT MEENA, 2401MC17

## Contents

- `diffs/` — unified patches against a pristine `xv6-public` source tree.
- `tests/` — user programs for the four questions.
- `screenshots/` — QEMU terminal evidence for each question.
- `Assignment_5_write_up.pdf` — concise technical write-up.

## Question 1 — Peterson's algorithm

`peterson.c` creates a parent and one child. The two processes use the shared variables
`flag[2]` and `turn`:

- `flag[i] = 1` announces that process `i` wants to enter.
- `turn = other` gives the other process priority when both want to enter.
- The process waits while the other flag is set and it is still the other process's turn.

No kernel lock is used for the actual mutual exclusion algorithm. Inside the critical
section, the program increments a shared counter and checks an `in_cs` marker for overlap.
Ten iterations per process should produce a final counter of 20 and zero violations.

## Question 2 — Producer-consumer

`prodcons.c` implements a circular buffer with capacity five. It uses three semaphores:

- `empty = 5` counts available buffer slots.
- `full = 0` counts items available for removal.
- `mutex = 1` protects the buffer indexes and data.

The producer inserts values 1 through 20. The consumer removes them in FIFO order. The
producer blocks when all five slots are occupied, and the consumer blocks when the buffer
is empty. The final check requires 20 produced items and 20 consumed items.

## Question 3 — Readers-writers

`readwrite.c` creates three reader processes and two writer processes. `read_count` is
protected by a reader mutex. The first reader acquires the shared resource semaphore and
the last reader releases it, allowing multiple readers to access the value concurrently.

Writers acquire the resource semaphore exclusively. A turnstile semaphore prevents a
continuous stream of new readers from postponing writers indefinitely. The program checks
that no writer overlaps with a reader or another writer, and expects final shared data of
8 from two writers performing four updates each.

## Question 4 — Dining philosophers

`dining.c` creates five philosopher processes and five fork semaphores. Each philosopher
repeats:

```text
THINKING -> HUNGRY -> EATING -> THINKING
```

The deadlock-avoidance strategy is **resource ordering**. Each philosopher acquires the
lower-numbered fork first and the higher-numbered fork second. Since all wait edges follow
the same increasing order, a circular wait cannot form. Each philosopher completes five
cycles, and the final completion counts verify progress for all five processes.

## Build and run

From a pristine xv6-public tree, apply the common kernel diffs first:

```sh
for f in diffs/q1_peterson_*.diff; do patch -p0 < "$f"; done
```

Then apply the relevant question test diff, or copy the corresponding file from `tests/`.
Build and run QEMU:

```sh
make TOOLPREFIX=i686-elf- clean
make TOOLPREFIX=i686-elf-
make TOOLPREFIX=i686-elf- fs.img
make TOOLPREFIX=i686-elf- QEMU=qemu-system-i386 CPUS=1 qemu-nox
```

At the xv6 shell, run:

```text
peterson
prodcons
readwrite
dining
```

## Verification results

- Peterson: final counter `20`, mutual-exclusion violations `0`.
- Producer-consumer: `20` values produced and `20` values consumed in FIFO order.
- Readers-writers: final shared value `8`, synchronization violations `0`.
- Dining philosophers: all five philosophers completed `5/5` cycles without deadlock.
