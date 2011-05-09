/*
 * Copyright (c) 2011 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */
#ifndef ESTATS_TYPES_H
#define ESTATS_TYPES_H

#if !defined(NULL)
# define NULL 0
#endif

typedef enum ESTATS_ADDRTYPE {
    ESTATS_ADDRTYPE_UNKNOWN = 0,
    ESTATS_ADDRTYPE_IPV4,
    ESTATS_ADDRTYPE_IPV6,
    ESTATS_ADDRTYPE_DNS = 16
} ESTATS_ADDRTYPE;

typedef enum ESTATS_AGENT_TYPE {
    ESTATS_AGENT_TYPE_LOCAL = 0,
} ESTATS_AGENT_TYPE;

typedef enum ESTATS_ERROR {
    ESTATS_ERR_SUCCESS              = 0,
    ESTATS_ERR_FILE                 = 1,
    ESTATS_ERR_AGENT_TYPE           = 2,
    ESTATS_ERR_NOMEM                = 3,
    ESTATS_ERR_NOCONNECTION         = 4,
    ESTATS_ERR_INVAL                = 5,
    ESTATS_ERR_HEADER               = 6,
    ESTATS_ERR_NOVAR                = 7,
    ESTATS_ERR_NOGROUP              = 8,
    ESTATS_ERR_SOCK                 = 9,
    ESTATS_ERR_KERNVER              = 10,
    ESTATS_ERR_NOIMPL               = 11,
    ESTATS_ERR_UNKNOWN              = 12,
    ESTATS_ERR_LIBC                 = 13,
    ESTATS_ERR_GROUP                = 14,
    ESTATS_ERR_CUSTOM               = 15,
    ESTATS_ERR_EOF                  = 16,
    ESTATS_ERR_UNKNOWN_TYPE         = 17,
    ESTATS_ERR_UNHANDLED_VALUE_TYPE = 18,
    ESTATS_ERR_RANGE                = 19,
    ESTATS_ERR_STRING_CONVERSION    = 20,
    ESTATS_ERR_ACCESS               = 21,
    ESTATS_ERR_CHKSUM               = 22,
} ESTATS_ERROR;

typedef enum ESTATS_TYPE {
    ESTATS_TYPE_INTEGER = 0,
    ESTATS_TYPE_INTEGER32 = 1,
    ESTATS_TYPE_INET_ADDRESS_IPV4 = 2,
    ESTATS_TYPE_IP_ADDRESS = ESTATS_TYPE_INET_ADDRESS_IPV4, /* Deprecated */
    ESTATS_TYPE_COUNTER32 = 3,
    ESTATS_TYPE_GAUGE32 = 4,
    ESTATS_TYPE_UNSIGNED32 = 5,
    ESTATS_TYPE_TIME_TICKS = 6,
    ESTATS_TYPE_COUNTER64 = 7,
    ESTATS_TYPE_INET_PORT_NUMBER = 8,
    ESTATS_TYPE_UNSIGNED16 = ESTATS_TYPE_INET_PORT_NUMBER, /* Deprecated */
    ESTATS_TYPE_INET_ADDRESS = 9,
    ESTATS_TYPE_INET_ADDRESS_IPV6 = 10,
    ESTATS_TYPE_OCTET = 12,
} ESTATS_TYPE;

typedef enum ESTATS_VALUE_TYPE {
    ESTATS_VALUE_TYPE_UINT16  = 0,
    ESTATS_VALUE_TYPE_INT32   = 1,
    ESTATS_VALUE_TYPE_UINT32  = 2,
    ESTATS_VALUE_TYPE_UINT64  = 3,
    ESTATS_VALUE_TYPE_STRING  = 4,
    ESTATS_VALUE_TYPE_IP4ADDR = 5,
    ESTATS_VALUE_TYPE_IP6ADDR = 6,
    ESTATS_VALUE_TYPE_OCTET   = 7,
} ESTATS_VALUE_TYPE;

struct estats_timeval {
    uint32_t sec;
    uint32_t usec;
};

struct estats_connection_spec {
    char dst_addr[17];
    uint16_t dst_port;
    char src_addr[17];
    uint16_t src_port;
};

struct spec_ascii {
    char dst_addr[INET6_ADDRSTRLEN];
    char dst_port[6];
    char src_addr[INET6_ADDRSTRLEN];
    char src_port[6];
};

typedef struct estats_agent      estats_agent;
typedef struct estats_connection estats_connection;
typedef struct estats_error      estats_error;
typedef struct estats_snapshot   estats_snapshot;
typedef struct estats_value      estats_value;
typedef struct estats_var        estats_var;

#endif /* ESTATS_TYPES_H */
