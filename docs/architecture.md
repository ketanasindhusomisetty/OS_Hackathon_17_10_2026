# System Architecture

## Overview
The Linux Command Execution Time Profiler executes a command in a child process and collects its execution statistics.

## Components
1. Command-line interface validates input.
2. Error pipe communicates execvp() failures.
3. fork() creates the child process.
4. execvp() executes the requested command.
5. wait4() collects child status and resource usage.
6. CLOCK_MONOTONIC measures elapsed time.
7. print_profile() displays the results.

## Execution Flow
Input command -> Validation -> Start time -> Pipe creation -> fork()
-> Execute command -> Collect status and resources -> End time -> Print report

## Error Handling
The program checks failures in pipe creation, fcntl(), clock_gettime(), fork(), read(), and wait4(). Missing commands are reported with exit status 127.
