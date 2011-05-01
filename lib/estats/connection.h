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
#ifndef ESTATS_CONNECTION_H
#define ESTATS_CONNECTION_H

estats_error* estats_connection_next( estats_connection **,
                                const estats_connection *);

estats_connection* estats_connection_return_next( const estats_connection *);

#define ESTATS_CONNECTION_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_connection_return_next(pos)) 

estats_error* estats_connection_get_agent( estats_agent **,
                                     const estats_connection *);

estats_error* estats_connection_get_cid( int * /* cid */,
                                   const estats_connection *);

estats_error* estats_connection_get_addrtype( ESTATS_ADDRTYPE *,
                                        const estats_connection *);

estats_error* estats_connection_get_connection_spec( struct estats_connection_spec *,
                                                     const estats_connection *);

estats_error* estats_connection_spec_as_strings( struct spec_ascii *,
                                                 struct estats_connection_spec *);

estats_error* estats_connection_read_access( const estats_connection *,
                                                   int /* mode */);

estats_error* estats_connection_read_value( estats_value **,
                                      const estats_connection *,
                                      const estats_var *);

estats_error* estats_connection_write_value( const estats_value *,
                                             const estats_connection *,
                                             const estats_var *);

estats_error* estats_connection_spec_compare( int * /* result */,
                                 const struct estats_connection_spec *,
                                 const struct estats_connection_spec *);

#endif /* ESTATS_CONNECTION_H */
