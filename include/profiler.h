
#ifndef PROFILER_H
#define PROFILER_H

#define _DEFAULT_SOURCE

#include <sys/resource.h>
#include <sys/types.h>
#include <time.h>

/* Stores command execution measurements. */
typedef struct {
    double elapsed_seconds;
    double user_cpu_seconds;
    double system_cpu_seconds;
    long peak_memory_kb;
    long minor_page_faults;
    long major_page_faults;
    long voluntary_context_switches;
    long involuntary_context_switches;
    int exit_status;
} ProfileResult;

/* Executes a command and collects its measurements. */
int profile_command(char *const argv[], ProfileResult *result);

/* Prints the collected measurements. */
void print_profile(const ProfileResult *result);

#endif
