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
#ifndef ESTATS_VAR_H
#define ESTATS_VAR_H

estats_error* estats_var_next( estats_var ** /* next */,
                         const estats_var *);

estats_var* estats_var_return_next( const estats_var *);

#define ESTATS_VAR_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_var_return_next(pos))

estats_error* estats_var_get_name( const char **,
                                   const estats_var *);

estats_error* estats_var_get_type( ESTATS_TYPE *,
                             const estats_var *);

estats_error* estats_var_get_size( size_t *,
                             const estats_var *);

#endif /* ESTATS_VAR_H */
