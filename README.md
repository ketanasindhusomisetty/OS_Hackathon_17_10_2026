# Linux Command Execution Time Profiler

## Overview
A command-line utility written in C that executes a Linux command and measures its execution time and resource usage.

## Features
- Measures elapsed wall-clock time using CLOCK_MONOTONIC.
- Reports user and system CPU time.
- Reports peak resident memory usage.
- Reports minor and major page faults.
- Reports voluntary and involuntary context switches.
- Preserves the executed command's exit status.
- Reports command execution errors, including missing commands.
- Uses fork(), execvp(), pipe(), wait4(), and getrusage-related resource data.

## Project Structure
- `src/main.c` — Main implementation.
- `include/profiler.h` — Data structure and function declarations.
- `tests/test_execution.sh` — Command execution and timing tests.
- `tests/test_errors.sh` — Error-handling tests.
- `tests/test_resources.sh` — Resource measurement tests.
- `docs/` — Architecture, module mapping, and test documentation.
- `Makefile` — Build and cleanup commands.

## Requirements
- Linux operating system
- GCC compiler
- GNU Make
- Bash

## Build
```bash
make clean && make
