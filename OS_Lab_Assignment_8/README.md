# OS Lab Assignment 8 — 

## Overview

This repository contains the complete submission for **Operating Systems Lab Assignment 8**, implemented using the `xv6-public` operating system.

The submission is organized into separate directories for the complete xv6 source code, modified kernel files, user-level test programs, and assignment documentation.

## Directory Structure

```text
OS-Lab-Assignment-8/
├── xv6/
│   └── Complete xv6-public source tree
│       with all assignment modifications
│
├── kernel-changes/
│   └── Only the kernel and build files
│       modified for this assignment
│
├── user-tests/
│   └── All eight required user programs
│
├── docs/
│   └── Consolidated assignment report
│
└── README.md
```

## Requirements

Before building and running xv6, ensure that the following tools are installed:

- GCC and the required development tools
- GNU Make
- QEMU
- The appropriate compiler toolchain for the xv6 target architecture

The exact dependencies may vary depending on your operating system and xv6 configuration.

## Build and Run Instructions

### 1. Navigate to the xv6 directory

```bash
cd xv6
```

### 2. Build xv6

Run the following command to compile the operating system:

```bash
make
```

### 3. Run xv6 using QEMU

```bash
make qemu
```

If your environment requires a specific cross-compiler or QEMU target, use the configuration supported by your xv6-public setup.

## Assignment Contents

### `xv6/`
Contains the complete, buildable `xv6-public` source tree, including all modifications made for Assignment 8.

### `kernel-changes/`
Contains only the kernel and build-related files modified as part of the assignment. This directory provides a focused view of the implementation changes.

### `user-tests/`
Contains all eight required user-level test programs used to test and demonstrate the assignment implementation.

### `docs/`
Contains the consolidated assignment report, including the implementation details, testing procedure, observations, and conclusions.

## Testing

After building and launching xv6:

1. Verify that the operating system boots successfully.
2. Run the required user-level test programs available in the xv6 image.
3. Check the output of each test program.
4. Compare the observed results with the expected assignment behavior.
5. Document the test results in the assignment report.

Ensure that the eight required test programs are included in the xv6 build configuration if they need to be executed inside QEMU.

## Submission Checklist

- [ ] Complete xv6-public source tree included.
- [ ] All assignment modifications included in `xv6/`.
- [ ] Modified kernel and build files included in `kernel-changes/`.
- [ ] All eight required user programs included in `user-tests/`.
- [ ] Consolidated assignment report included in `docs/`.
- [ ] xv6 builds successfully using `make`.
- [ ] xv6 runs successfully using `make qemu`.
- [ ] Required test programs have been tested.

## Notes

- Keep the `xv6/` directory self-contained and buildable.
- The `kernel-changes/` directory is intended as a reference for reviewing modifications; it does not replace the complete source tree.
- Run build commands from inside the `xv6/` directory.
- The expected output and test results should be documented in the assignment report.

## Conclusion

This submission provides an organized structure for reviewing, building, and testing the xv6-public implementation for Operating Systems Lab Assignment 8.
