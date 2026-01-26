#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>
#include <stddef.h>

#define C_OK 0

#define ERR_INVALID_VITALS_PTR -1
#define ERR_INVALID_BUFFER_PTR -2
#define ERR_INVALID_FILE_PATH_PTR -3
#define ERR_FAILED_TO_OPEN_FILE -4
#define ERR_FAILED_TO_READ_INTO_BUFFER -5

#define KERNEL_VERSION_PATH "/proc/version"
#define CPU_USAGE_SLEEP_MS 5000

typedef struct {
    double cpu_usage_percentage;
    uint64_t total_mem_kb;
    uint64_t free_mem_kb;
    uint64_t uptime_seconds;
    uint16_t network_interface_count;
} SystemVitals;

/**
 * Injects the vitals of the system into the given pointer to `vitals` struct.
 * 
 * @param vitals A pointer to a vitals struct.
 * 
 * @returns If successful, `C_OK`.
 * @returns If unsuccessful, an error code.
 */
int collect_vitals(SystemVitals *vitals);

/**
 * Injects the kernel version into the given buffer of size `size`.
 * 
 * @param buffer The buffer injected into.
 * @param size The size of the `buffer`.
 * 
 * @returns If successful, `C_OK`.
 * @returns If unsuccessful, an error code.
 */
int get_kernel_version(char *buffer, size_t size);

#endif