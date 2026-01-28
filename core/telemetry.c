#include "telemetry.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

// Puts `buffer_size`-1 characters from the file at `file_path` into `buffer`.
static int read_lines_into_buffer(const char *file_path, char *buffer, size_t buffer_size, size_t line_count);

// Calculates the CPU Usage over `time_ms` ms in time.
int calculate_cpu_usage(double *cpu_usage, int time_ms);

// Fetches the memory stats and injects them into the given pointers.
int fetch_mem_stats(uint64_t *total_mem_kb, uint64_t *free_mem_kb, uint64_t *available_mem_kb);

// Injects the jiffies into the `jiffies` array.
static int inject_jiffies(long int jiffies[2]);

int collect_vitals(SystemVitals *vitals) {
    if(!vitals) return ERR_INVALID_VITALS_PTR;

    return C_OK;
}

int get_kernel_version(char *buffer, size_t size) {
    if(!buffer) return ERR_INVALID_ARG_PTR;

    int res = read_lines_into_buffer(KERNEL_VERSION_PATH, buffer, size, 1);
    if(res != C_OK) return res;

    // remove newline
    char *nl;
    if((nl = strrchr(buffer, '\n')) != NULL) {
        *nl = '\0';
    }

    return C_OK;
}

int read_lines_into_buffer(const char *file_path, char *buffer, size_t buffer_size, size_t line_count) {
    if(!file_path) return ERR_INVALID_FILE_PATH_PTR;
    if(!buffer) return ERR_INVALID_ARG_PTR;

    // considering I'm only using this to read text files in /proc atm, 'r' should be a good enough mode
    FILE *fPtr = fopen(file_path, "r");
    if(!fPtr) return ERR_FAILED_TO_OPEN_FILE;

    char tmp_buffer[READ_PROC_FILE_BUFFER];

    // read 'line_count' lines from file
    while(fgets(tmp_buffer, READ_PROC_FILE_BUFFER, fPtr) && line_count) {
        strcat(buffer, tmp_buffer);
        line_count--;
    }

    if(line_count != 0) return ERR_FAILED_TO_READ_INTO_BUFFER;

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

    char buffer[READ_PROC_FILE_BUFFER] = {0};
    if((res = read_lines_into_buffer("/proc/stat", buffer, READ_PROC_FILE_BUFFER, 1)) != C_OK) {
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

int fetch_mem_stats(uint64_t *total_mem_kb, uint64_t *free_mem_kb, uint64_t *available_mem_kb) {
    if(!total_mem_kb || !free_mem_kb || !available_mem_kb) return ERR_INVALID_ARG_PTR;

    int res;
    char buffer[READ_PROC_FILE_BUFFER] = {0};
    
    if((res = read_lines_into_buffer("/proc/meminfo", buffer, READ_PROC_FILE_BUFFER, 3)) != C_OK) {
        return res;
    }

    char *save_lines;
    char *lines = strtok_r(buffer, "\n", &save_lines);
    
    char *dup; // TODO: is there a better alternative than strdup()? (b/c strtok'ing for two diff. things just breaks everything)
    char *dup_sep;
    char *save_dup;

    size_t reading = 0; // 0 = memtotal, 1 = memfree, 2 = memavailable

    uint64_t val;
    char *end;

    while(lines != NULL) {
        dup = strdup(lines);
        if(!dup) return ERR_FAILED_TO_DUPLICATE_PROC_LINE;

        dup_sep = strtok_r(dup, ":", &save_dup);
        dup_sep = strtok_r(NULL, ":", &save_dup); // skip key

        val = strtoull(dup_sep, &end, 10);

        if(reading == 0) {
            *total_mem_kb = val;
        } else if(reading == 1) {
            *free_mem_kb = val;
        } else {
            *available_mem_kb = val;
        }

        free(dup);
        lines = strtok_r(NULL, "\n", &save_lines);
        reading++;
    }

    return C_OK;
}