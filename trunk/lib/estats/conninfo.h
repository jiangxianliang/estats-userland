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
#if !defined(ESTATS_CONNINFO_H)
#define ESTATS_CONNINFO_H

#define ESTATS_CMDLINE_LEN_MAX 32

typedef struct estats_conninfo {
    char                           cmdline[ESTATS_CMDLINE_LEN_MAX];
    int                            cid;
    pid_t                          pid;
    uid_t                          uid;
    ino_t                          ino;
    int                            state;
    ESTATS_ADDRTYPE                addrtype;
    struct estats_connection_spec  spec;
    struct estats_conninfo        *next;
} estats_conninfo;

estats_error* estats_get_conninfo_head(estats_conninfo** _head, estats_agent* _agent);

estats_error* estats_conninfo_next(estats_conninfo** _next, const estats_conninfo* _prev);

estats_conninfo* estats_conninfo_return_next(const estats_conninfo* _prev);

void          estats_conninfo_free(estats_conninfo** _conninfo);

estats_error* estats_conninfo_get_cid(int* _cid,
                                  const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_pid(int* _pid,
                                  const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_uid(int* _uid,
                                  const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_state(int* _state,
                                    const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_cmdline(char** _cmdline,
                                      const estats_conninfo* _conninfo);

#define ESTATS_CONNINFO_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_conninfo_return_next(pos))

#endif /* !defined(ESTATS_CONNINFO_H) */
