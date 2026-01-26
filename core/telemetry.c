#include "telemetry.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

// Puts `buffer_size`-1 characters from the file at `file_path` into `buffer`.
static int read_line_into_buffer(const char *file_path, char *buffer, size_t buffer_size);

// Calculates the CPU Usage over `CPU_USAGE_SLEEP_MS` ms in time.
static int calculate_cpu_usage();

int collect_vitals(SystemVitals *vitals) {
    if(!vitals) return ERR_INVALID_VITALS_PTR;

    return C_OK;
}

int get_kernel_version(char *buffer, size_t size) {
    if(!buffer) return ERR_INVALID_BUFFER_PTR;

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
    if(!buffer) return ERR_INVALID_BUFFER_PTR;

    // considering I'm only using this to read text files in /proc atm, 'r' should be a good enough mode
    FILE *fPtr = fopen(file_path, "r");
    if(!fPtr) return ERR_FAILED_TO_OPEN_FILE;

    // read 'buffer_size'-1 characters into bufer
    if(!fgets(buffer, buffer_size, fPtr)) return ERR_FAILED_TO_READ_INTO_BUFFER;

    fclose(fPtr);

    return C_OK;
}