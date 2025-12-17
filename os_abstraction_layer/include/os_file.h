/**
 * os_file.h - OS Abstraction Layer File Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * This file defines the file abstraction interface.
 */

#ifndef __OS_FILE_H__
#define __OS_FILE_H__

#include "os_abstract.h"

#ifdef __cplusplus
extern "C" {
#endif

/* File handle */
typedef struct os_file os_file_t;

/* File open modes */
typedef enum {
    OS_FILE_MODE_READ = 0x01,
    OS_FILE_MODE_WRITE = 0x02,
    OS_FILE_MODE_APPEND = 0x04,
    OS_FILE_MODE_CREATE = 0x08,
    OS_FILE_MODE_TRUNCATE = 0x10,
    OS_FILE_MODE_BINARY = 0x20,
} os_file_mode_t;

/* File seek origins */
typedef enum {
    OS_FILE_SEEK_SET,
    OS_FILE_SEEK_CUR,
    OS_FILE_SEEK_END,
} os_file_seek_t;

/**
 * Open a file
 * @param path File path
 * @param mode Open mode (bitwise OR of os_file_mode_t)
 * @return File handle on success, NULL on error
 */
os_file_t *os_file_open(const char *path, uint32_t mode);

/**
 * Close a file
 * @param file File handle
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_file_close(os_file_t *file);

/**
 * Read from file
 * @param file File handle
 * @param buffer Buffer to store data
 * @param size Number of bytes to read
 * @param[out] bytes_read Actual number of bytes read
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_file_read(os_file_t *file, void *buffer, size_t size, size_t *bytes_read);

/**
 * Write to file
 * @param file File handle
 * @param buffer Data to write
 * @param size Number of bytes to write
 * @param[out] bytes_written Actual number of bytes written
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_file_write(os_file_t *file, const void *buffer, size_t size, size_t *bytes_written);

/**
 * Seek in file
 * @param file File handle
 * @param offset Offset from origin
 * @param origin Seek origin
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_file_seek(os_file_t *file, int64_t offset, os_file_seek_t origin);

/**
 * Get file size
 * @param file File handle
 * @param[out] size File size in bytes
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_file_size(os_file_t *file, size_t *size);

/**
 * Check if file exists
 * @param path File path
 * @return true if exists, false otherwise
 */
bool os_file_exists(const char *path);

/**
 * Delete a file
 * @param path File path
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_file_delete(const char *path);

#ifdef __cplusplus
}
#endif

#endif /* __OS_FILE_H__ */
