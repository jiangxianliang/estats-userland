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
#ifndef ESTATS_SNAPSHOT_H
#define ESTATS_SNAPSHOT_H

estats_error* estats_snapshot_alloc( estats_snapshot **,
                                     estats_connection *);

void          estats_snapshot_free( estats_snapshot **);

estats_error* estats_take_snapshot( estats_snapshot *);

estats_error* estats_snapshot_read_value( estats_value **,
                                    const estats_snapshot *,
                                    const estats_var *);

estats_error* estats_snapshot_get_timeval(struct estats_timeval *,
                                                 estats_snapshot *);

estats_error* estats_snapshot_delta( estats_value **,
                               const estats_snapshot *,
                               const estats_snapshot *,
                               const estats_var *);

#endif /* ESTATS_SNAPSHOT_H */
