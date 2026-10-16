# Module Mapping

| Module | Responsibility | Implementation |
|---|---|---|
| Input handling | Validate command-line arguments | main() |
| Process creation | Create child process | fork() |
| Command execution | Run the requested command | execvp() |
| Error handling | Detect execution failures | pipe(), fcntl(), read() |
| Time measurement | Measure elapsed time | clock_gettime() |
| Resource measurement | Collect CPU and memory statistics | wait4(), struct rusage |
| Exit status | Preserve command result | WIFEXITED(), WEXITSTATUS() |
| Reporting | Display measurements | print_profile() |
| Testing | Verify execution and errors | tests/*.sh |
