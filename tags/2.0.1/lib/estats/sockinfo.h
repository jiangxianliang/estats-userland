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
#ifndef ESTATS_SOCKINFO_H
#define ESTATS_SOCKINFO_H


typedef struct estats_sockinfo estats_sockinfo;
typedef struct estats_sockinfo_item estats_sockinfo_item;

estats_error* estats_sockinfo_get_list_head( estats_sockinfo_item **,
                                             estats_sockinfo *);

estats_error* estats_sockinfo_item_next( estats_sockinfo_item **,
                                   const estats_sockinfo_item* _prev);

#define ESTATS_SOCKINFO_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_sockinfo_item_return_next(pos))

estats_sockinfo_item* estats_sockinfo_item_return_next(const estats_sockinfo_item* _prev);

estats_error* estats_sockinfo_new(estats_sockinfo **, estats_agent *);

void          estats_sockinfo_free(estats_sockinfo** _sockinfo);

estats_error* estats_sockinfo_get_cid(int* _cid,
                                  const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_pid(int* _pid,
                                  const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_uid(int* _uid,
                                  const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_state(int* _state,
                                    const estats_sockinfo_item* _sockinfo);

estats_error* estats_sockinfo_get_cmdline(char** _cmdline,
                                      const estats_sockinfo_item* _sockinfo);
estats_error* estats_sockinfo_get_connection_spec( struct estats_connection_spec *,
                                                    const estats_sockinfo_item *);

#endif /* ESTATS_SOCKINFO_H */
