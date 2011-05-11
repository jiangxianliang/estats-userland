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
#ifndef ESTATS_LOG_H
#define ESTATS_LOG_H


estats_error* estats_log_open( estats_log **,
                         const char * /* path */,
                         const char * /* mode */);

estats_error* estats_log_close( estats_log **);

estats_error* estats_log_read_entry( estats_log_entry **,
                                     estats_log *);

void          estats_log_entry_free( estats_log_entry **);

estats_error* estats_log_write_entry( estats_log *,
                                      estats_snapshot *);

estats_error* estats_log_find_var_from_name( estats_var **,
                                       const estats_log *,
                                       const char * /* name */);

estats_error* estats_log_entry_read_value( estats_value **,
                                     const estats_log_entry *,
                                     const estats_var *);

estats_error* estats_log_entry_read_timestamp( struct estats_timeval *,
                                                const estats_log_entry *);

estats_error* estats_log_read_all_entries( estats_log *);

estats_error* estats_log_get_entry_head( estats_log_entry **,
                                         estats_log *);

estats_error* estats_log_entry_next( estats_log_entry **,
                               const estats_log_entry *);

estats_log_entry* estats_log_entry_return_next( const estats_log_entry *);

#define ESTATS_LOG_ENTRY_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_log_entry_return_next(pos))

#endif /* ESTATS_LOG_H */
