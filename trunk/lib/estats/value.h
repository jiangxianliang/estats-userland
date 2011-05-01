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
#ifndef ESTATS_VALUE_H
#define ESTATS_VALUE_H

estats_error* estats_value_new( estats_value **,
                          const void * /* buf */,
                                ESTATS_VALUE_TYPE);

void          estats_value_free( estats_value **);

estats_error* estats_value_get_type( ESTATS_VALUE_TYPE *,
                               const estats_value *);

estats_error* estats_value_change_type( estats_value **,
                                        ESTATS_VALUE_TYPE);

estats_error* estats_value_as_uint16( uint16_t *,
                                const estats_value *);

estats_error* estats_value_as_int32( int32_t *,
                               const estats_value *);

estats_error* estats_value_as_uint32( uint32_t *,
                                const estats_value *);

estats_error* estats_value_as_uint64( uint64_t *,
                                const estats_value *);

estats_error* estats_value_as_string( char **,
                                const estats_value *);

estats_error* estats_value_as_ip4addr( struct in_addr *,
                                       const estats_value *);

estats_error* estats_value_as_ip6addr( struct in6_addr *,
                                       const estats_value *);

estats_error* estats_value_compare( int *,
                              const estats_value *,
                              const estats_value *);

estats_error* estats_value_difference( estats_value ** /* result */,
                                 const estats_value *,
                                 const estats_value *);

#endif /* ESTATS_VALUE_H */
