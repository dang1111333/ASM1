# ASM1 - Embedded Linux Assignment 1

## 1. Overview

This assignment focuses on Linux kernel programming and device driver development in an Embedded Linux environment.

## 2. Objectives

- Understand Linux kernel module development.
- Implement device driver functionality.
- Learn how to handle IOCTL requests.
- Compile and test kernel modules for the target architecture.
- Understand communication between user space and kernel space.

## 3. Technologies Used

- Ubuntu Linux
- Linux Kernel 5.15
- ARM Cross-Compilation Toolchain
- GNU Make
- C Programming Language
- QEMU

## 4. Project Structure

- `task1_ioctl/`: Source files and build configuration for Task 1.
- `README.md`: Assignment documentation.

Additional tasks and reports can be documented here as they are added to the repository.

## 5. Build Instructions

Navigate to the task directory:

```bash
cd task1_ioctl
```

Build the kernel module using the provided Makefile.

Example:

```bash
make
```

The build configuration should specify the correct kernel source directory, architecture, and cross-compiler.

## 6. Testing

Testing should be performed in a compatible Embedded Linux environment.

The testing procedure may include:

- Loading the kernel module.
- Checking kernel messages.
- Testing IOCTL operations.
- Verifying returned values and error handling.
- Unloading the module.

## 7. Learning Outcomes

- Practice Linux kernel programming.
- Understand IOCTL-based communication.
- Gain experience with cross-compilation.
- Improve debugging and testing skills.

## 8. Author

**Name:** Le Minh Dang  
**University:** FPT University (FPTU)
