
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
#if !defined(ESTATS_LOG_INT_H)
#define ESTATS_LOG_INT_H

#include <estats/estats-int.h>

union ltv {
    int32_t s32_val;
    int64_t s64_val;
};

struct estats_log_timeval {
    union ltv sec;
    union ltv usec;
};

struct estats_log_entry {
    void*  data;
    struct estats_list list;
    struct estats_log* log;
    struct estats_log_timeval timeval;
};

typedef enum ESTATS_LOG_MODE {
    R_MODE,
    W_MODE
} ESTATS_LOG_MODE;

struct estats_log {
    FILE*               fp;
    int                 swap;
    int                 tvsize;
    int                 bufsize;
    int                 nvars;
    struct estats_list  var_list_head;
    struct estats_list  entry_list_head;
    ESTATS_LOG_MODE     mode;
};

#include <endian.h>
#include <byteswap.h>

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define LOG_HOST_ORDER 0
#else
#define LOG_HOST_ORDER 1
# endif

#endif /* !defined(ESTATS_LOG_INT_H) */

