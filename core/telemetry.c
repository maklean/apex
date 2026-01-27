#include "telemetry.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// Puts `buffer_size`-1 characters from the file at `file_path` into `buffer`.
static int read_line_into_buffer(const char *file_path, char *buffer, size_t buffer_size);

// Calculates the CPU Usage over `time_ms` ms in time.
int calculate_cpu_usage(double *cpu_usage, int time_ms);

// Injects the jiffies into the `jiffies` array.
static int inject_jiffies(long int jiffies[2]);

int collect_vitals(SystemVitals *vitals) {
    if(!vitals) return ERR_INVALID_VITALS_PTR;

    return C_OK;
}

int get_kernel_version(char *buffer, size_t size) {
    if(!buffer) return ERR_INVALID_ARG_PTR;

    int res = read_line_into_buffer(KERNEL_VERSION_PATH, buffer, size);
    if(res != C_OK) return res;

    // remove newline
    char *nl;
    if((nl = strrchr(buffer, '\n')) != NULL) {
        *nl = '\0';
    }

    return C_OK;
}

int read_line_into_buffer(const char *file_path, char *buffer, size_t buffer_size) {
    if(!file_path) return ERR_INVALID_FILE_PATH_PTR;
    if(!buffer) return ERR_INVALID_ARG_PTR;

    // considering I'm only using this to read text files in /proc atm, 'r' should be a good enough mode
    FILE *fPtr = fopen(file_path, "r");
    if(!fPtr) return ERR_FAILED_TO_OPEN_FILE;

    // read 'buffer_size'-1 characters into bufer
    if(!fgets(buffer, buffer_size, fPtr)) return ERR_FAILED_TO_READ_INTO_BUFFER;

    fclose(fPtr);

    return C_OK;
}

int calculate_cpu_usage(double *cpu_usage, int time_ms) {
    if(!cpu_usage) return ERR_INVALID_ARG_PTR;

    int res;

    // inject into a, wait for some abitrary amount of time, then inject into b
    long int a[2];
    if((res = inject_jiffies(a)) != 0) {
        return res;
    }

    usleep(time_ms * 1000);

    long int b[2];
    if((res = inject_jiffies(b)) != 0) {
        return res;
    }

    // this should give the CPU usage (source: https://stackoverflow.com/a/3017438)
    long work_over_period = b[1] - a[1];
    long total_over_period = b[0] - a[0];

    *cpu_usage = (double)work_over_period / total_over_period * 100;

    return C_OK;
}

int inject_jiffies(long int jiffies[2]) {
    if(!jiffies) return ERR_INVALID_ARG_PTR;

    int res;

    char buffer[CPU_USAGE_BUFFER];
    if((res = read_line_into_buffer("/proc/stat", buffer, CPU_USAGE_BUFFER)) != 0) {
        return res;
    }

    char *buf_sep = strtok(buffer, " ");
    buf_sep = strtok(NULL, " "); // skip "cpu"

    long int total = 0; // sum of everything.
    long int work = 0; // sum of normal and niced processes in user mode, and processes in kernel mode (first 3)

    // TODO: should probably iterate a specific amount of times instead of waiting for buf_sep to hit null
    size_t i = 0;
    long int n;
    while(buf_sep != NULL) {
        n = atol(buf_sep);

        total += n;
        if(i < 3) work += n;

        buf_sep = strtok(NULL, " ");
        i++;
    }

    jiffies[0] = total;
    jiffies[1] = work;

    return C_OK;
}