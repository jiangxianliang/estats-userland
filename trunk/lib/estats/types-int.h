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
#ifndef ESTATS_TYPES_INT_H
#define ESTATS_TYPES_INT_H


#define ESTATS_VERSTR_LEN_MAX 64
#define ESTATS_GROUPNAME_LEN_MAX 32
#define ESTATS_VARNAME_LEN_MAX 32
#define ESTATS_VAR_FL_DEP    1
#define ESTATS_VAR_FL_WARNED 2

struct estats_list {
    struct estats_list* next;
    struct estats_list* prev;
};

struct estats_connection {
    struct estats_connection_spec    spec;
    struct estats_list               list;
    struct estats_agent*             agent;
    int                              cid;
    ESTATS_ADDRTYPE                  addrtype;
};

struct estats_var {
    char                 name[ESTATS_VARNAME_LEN_MAX];
    struct estats_list   list;
    struct estats_group* group;
    size_t               len;
    int                  type;
    int                  offset;
    int                  flags;
};

struct estats_group {
    char                 name[ESTATS_GROUPNAME_LEN_MAX];
    struct estats_list   list;
    struct estats_agent* agent;
    int                  size;
    int                  nvars;
    struct estats_list   var_list_head;
};

struct estats_agent {
    char                      version[ESTATS_VERSTR_LEN_MAX];
    struct estats_group*      spec;
    struct estats_group*      read;
    struct estats_list        connection_list_head;
    ESTATS_AGENT_TYPE         type;
};

struct estats_snapshot {
    struct estats_connection_spec  spec;
    struct estats_timeval          tv;
    struct estats_group*           group;
    int                            cid;
    void*                          data;
};

struct estats_error {
    ESTATS_ERROR num;
    const char* msg;
    char* xtra;
    const char* file;
    int line;
    const char* function;
};

struct estats_value {
    ESTATS_VALUE_TYPE type;
    union {
        uint8_t         u8_val;
        uint16_t        u16_val;
        int32_t         s32_val;
        uint32_t        u32_val;
        uint64_t        u64_val;
        char*           str_val;
        struct in_addr  ip4addr_val;
        struct in6_addr ip6addr_val;
    } u;
};

struct estats_log_entry {
    struct estats_list    list;
    struct estats_timeval tv;
    void*                 data;
    struct estats_log*    log;
};

typedef enum ESTATS_LOG_MODE {
    R_MODE,
    W_MODE
} ESTATS_LOG_MODE;

struct estats_log {
    struct estats_list  var_list_head;
    struct estats_list  entry_list_head;
    FILE*               fp;
    int                 swap;
    int                 bufsize;
    int                 nvars;
    ESTATS_LOG_MODE     mode;
};

typedef struct estats_group estats_group;


#endif /* ESTATS_TYPES_INT_H */
