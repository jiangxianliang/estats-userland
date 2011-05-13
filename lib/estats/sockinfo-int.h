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
#ifndef ESTATS_SOCKINFO_INT_H
#define ESTATS_SOCKINFO_INT_H


#define ESTATS_CMDLINE_LEN_MAX 32

struct estats_sockinfo {
    struct estats_agent*  agent;
    struct estats_list    list_head;
};

struct estats_sockinfo_item {
    struct estats_connection_spec  spec;
    char                           cmdline[ESTATS_CMDLINE_LEN_MAX];
    struct estats_list             list;
    struct estats_sockinfo*        sockinfo;
    pid_t                          pid;
    uid_t                          uid;
    ino_t                          ino;
    int                            state;
    int                            cid;
    ESTATS_ADDRTYPE                addrtype;
};


#endif /* ESTATS_SOCKINFO_INT_H */
