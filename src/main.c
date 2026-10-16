#include "profiler.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static double seconds_between(struct timespec start,
                              struct timespec end)
{
    return (double)(end.tv_sec - start.tv_sec) +
           (double)(end.tv_nsec - start.tv_nsec) / 1e9;
}

static double timeval_seconds(struct timeval value)
{
    return (double)value.tv_sec +
           (double)value.tv_usec / 1e6;
}

int profile_command(char *const argv[], ProfileResult *result)
{
    if (argv == NULL || argv[0] == NULL || result == NULL) {
        fprintf(stderr, "Error: invalid command or result pointer.\n");
        return -1;
    }

    memset(result, 0, sizeof(*result));

    int error_pipe[2];

    if (pipe(error_pipe) == -1) {
        perror("pipe");
        return -1;
    }

    int flags = fcntl(error_pipe[1], F_GETFD);

    if (flags == -1 ||
        fcntl(error_pipe[1], F_SETFD, flags | FD_CLOEXEC) == -1) {
        perror("fcntl");
        close(error_pipe[0]);
        close(error_pipe[1]);
        return -1;
    }

    struct timespec start, end;

    if (clock_gettime(CLOCK_MONOTONIC, &start) == -1) {
        perror("clock_gettime");
        close(error_pipe[0]);
        close(error_pipe[1]);
        return -1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(error_pipe[0]);
        close(error_pipe[1]);
        return -1;
    }

    if (pid == 0) {
        close(error_pipe[0]);

        execvp(argv[0], argv);

        int exec_error = errno;
        ssize_t written;

        do {
            written = write(error_pipe[1], &exec_error,
                            sizeof(exec_error));
        } while (written == -1 && errno == EINTR);

        _exit(127);
    }

    close(error_pipe[1]);

    int exec_error = 0;
    size_t received = 0;
    int read_failed = 0;
    char *error_bytes = (char *)&exec_error;

    while (received < sizeof(exec_error)) {
        ssize_t n = read(error_pipe[0],
                         error_bytes + received,
                         sizeof(exec_error) - received);

        if (n > 0) {
            received += (size_t)n;
        } else if (n == 0) {
            break;
        } else if (errno != EINTR) {
            perror("read");
            read_failed = 1;
            break;
        }
    }

    close(error_pipe[0]);

    int status;
    struct rusage usage;
    pid_t waited;

    do {
        waited = wait4(pid, &status, 0, &usage);
    } while (waited == -1 && errno == EINTR);

    if (waited == -1) {
        perror("wait4");
        return -1;
    }

    if (read_failed) {
        return -1;
    }

    if (clock_gettime(CLOCK_MONOTONIC, &end) == -1) {
        perror("clock_gettime");
        return -1;
    }

    result->elapsed_seconds = seconds_between(start, end);
    result->user_cpu_seconds = timeval_seconds(usage.ru_utime);
    result->system_cpu_seconds = timeval_seconds(usage.ru_stime);
    result->peak_memory_kb = usage.ru_maxrss;
    result->minor_page_faults = usage.ru_minflt;
    result->major_page_faults = usage.ru_majflt;
    result->voluntary_context_switches = usage.ru_nvcsw;
    result->involuntary_context_switches = usage.ru_nivcsw;

    if (received == sizeof(exec_error)) {
        fprintf(stderr, "Cannot execute '%s': %s\n",
                argv[0], strerror(exec_error));
        result->exit_status = 127;
        return 1;
    }

    if (received != 0) {
        fprintf(stderr, "Error: incomplete execution error message.\n");
        return -1;
    }

    if (WIFEXITED(status)) {
        result->exit_status = WEXITSTATUS(status);
    } else if (WIFSIGNALED(status)) {
        result->exit_status = 128 + WTERMSIG(status);
    } else {
        result->exit_status = -1;
    }

    return 0;
}

void print_profile(const ProfileResult *result)
{
    printf("\n========== COMMAND PROFILE ==========\n");
    printf("Elapsed time:                 %.6f seconds\n",
           result->elapsed_seconds);
    printf("User CPU time:                %.6f seconds\n",
           result->user_cpu_seconds);
    printf("System CPU time:              %.6f seconds\n",
           result->system_cpu_seconds);
    printf("Peak resident memory:         %ld KB\n",
           result->peak_memory_kb);
    printf("Minor page faults:             %ld\n",
           result->minor_page_faults);
    printf("Major page faults:             %ld\n",
           result->major_page_faults);
    printf("Voluntary context switches:    %ld\n",
           result->voluntary_context_switches);
    printf("Involuntary context switches:  %ld\n",
           result->involuntary_context_switches);
    printf("Exit status:                   %d\n",
           result->exit_status);

    if (result->exit_status == 0) {
        printf("Result: Command completed successfully.\n");
    } else {
        printf("Result: Command exited unsuccessfully or was terminated.\n");
    }

    printf("=====================================\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,
                "Usage: %s <command> [arguments...]\n"
                "Example: %s ls -l\n",
                argv[0], argv[0]);
        return 2;
    }

    ProfileResult result;
    int profile_status = profile_command(&argv[1], &result);

    if (profile_status == -1) {
        return 1;
    }

    if (profile_status == 1) {
        /* execvp failed: report the startup error without a
           misleading command-execution profile. */
        return 127;
    }

    print_profile(&result);
    return result.exit_status;
}
