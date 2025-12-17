/**
 * os_network.h - OS Abstraction Layer Network Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * This file defines the network abstraction interface (basic socket operations).
 */

#ifndef __OS_NETWORK_H__
#define __OS_NETWORK_H__

#include "os_abstract.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Socket handle */
typedef struct os_socket os_socket_t;

/* Socket types */
typedef enum {
    OS_SOCKET_TYPE_STREAM,    /* TCP */
    OS_SOCKET_TYPE_DATAGRAM,  /* UDP */
    OS_SOCKET_TYPE_RAW,
} os_socket_type_t;

/* Socket address family */
typedef enum {
    OS_SOCKET_AF_INET,        /* IPv4 */
    OS_SOCKET_AF_INET6,       /* IPv6 */
} os_socket_af_t;

/* Socket address (IPv4) */
typedef struct {
    uint32_t addr;            /* IP address in network byte order */
    uint16_t port;            /* Port in network byte order */
} os_socket_addr_ipv4_t;

/* Socket address (generic) */
typedef union {
    os_socket_addr_ipv4_t ipv4;
    /* IPv6 could be added later */
} os_socket_addr_t;

/**
 * Create a socket
 * @param family Address family
 * @param type Socket type
 * @return Socket handle on success, NULL on error
 */
os_socket_t *os_socket_create(os_socket_af_t family, os_socket_type_t type);

/**
 * Close a socket
 * @param socket Socket handle
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_close(os_socket_t *socket);

/**
 * Bind socket to address
 * @param socket Socket handle
 * @param addr Address
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_bind(os_socket_t *socket, const os_socket_addr_t *addr);

/**
 * Listen for connections (TCP)
 * @param socket Socket handle
 * @param backlog Maximum pending connections
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_listen(os_socket_t *socket, int backlog);

/**
 * Accept a connection (TCP)
 * @param socket Socket handle
 * @param[out] client_addr Client address (optional, can be NULL)
 * @return New socket handle for the connection, NULL on error
 */
os_socket_t *os_socket_accept(os_socket_t *socket, os_socket_addr_t *client_addr);

/**
 * Connect to remote address (TCP)
 * @param socket Socket handle
 * @param addr Remote address
 * @param timeout_ms Timeout in milliseconds
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_connect(os_socket_t *socket, const os_socket_addr_t *addr, uint32_t timeout_ms);

/**
 * Send data (TCP)
 * @param socket Socket handle
 * @param data Data to send
 * @param size Data size
 * @param timeout_ms Timeout in milliseconds
 * @param[out] bytes_sent Actual bytes sent
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_send(os_socket_t *socket, const void *data, size_t size,
                          uint32_t timeout_ms, size_t *bytes_sent);

/**
 * Receive data (TCP)
 * @param socket Socket handle
 * @param buffer Buffer to store data
 * @param size Buffer size
 * @param timeout_ms Timeout in milliseconds
 * @param[out] bytes_received Actual bytes received
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_recv(os_socket_t *socket, void *buffer, size_t size,
                          uint32_t timeout_ms, size_t *bytes_received);

/**
 * Send datagram (UDP)
 * @param socket Socket handle
 * @param data Data to send
 * @param size Data size
 * @param addr Destination address (optional for connected sockets)
 * @param timeout_ms Timeout in milliseconds
 * @param[out] bytes_sent Actual bytes sent
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_sendto(os_socket_t *socket, const void *data, size_t size,
                            const os_socket_addr_t *addr, uint32_t timeout_ms,
                            size_t *bytes_sent);

/**
 * Receive datagram (UDP)
 * @param socket Socket handle
 * @param buffer Buffer to store data
 * @param size Buffer size
 * @param[out] addr Source address (optional, can be NULL)
 * @param timeout_ms Timeout in milliseconds
 * @param[out] bytes_received Actual bytes received
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_recvfrom(os_socket_t *socket, void *buffer, size_t size,
                              os_socket_addr_t *addr, uint32_t timeout_ms,
                              size_t *bytes_received);

/**
 * Set socket option
 * @param socket Socket handle
 * @param level Option level
 * @param option Option name
 * @param value Option value
 * @param value_size Size of option value
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_setsockopt(os_socket_t *socket, int level, int option,
                                const void *value, size_t value_size);

/**
 * Get socket option
 * @param socket Socket handle
 * @param level Option level
 * @param option Option name
 * @param[out] value Option value
 * @param[in,out] value_size Size of option value buffer (in), actual size (out)
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_getsockopt(os_socket_t *socket, int level, int option,
                                void *value, size_t *value_size);

/**
 * Convert IP address string to binary
 * @param ip_str IP address string (e.g., "192.168.1.1")
 * @param[out] addr Binary address
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_ipstr_to_addr(const char *ip_str, os_socket_addr_ipv4_t *addr);

/**
 * Convert binary address to string
 * @param addr Binary address
 * @param[out] buffer String buffer
 * @param buffer_size Buffer size (should be at least 16 bytes for IPv4)
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_socket_addr_to_ipstr(const os_socket_addr_ipv4_t *addr,
                                   char *buffer, size_t buffer_size);

#ifdef __cplusplus
}
#endif

#endif /* __OS_NETWORK_H__ */
