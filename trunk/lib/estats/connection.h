
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
#if !defined(ESTATS_CONNECTION_H)
#define ESTATS_CONNECTION_H

estats_error* estats_connection_next(estats_connection** _next,
                                     const estats_connection* _prev);

estats_error* estats_connection_get_agent(estats_agent** _agent,
                                          const estats_connection* _conn);
estats_error* estats_connection_get_cid(int* _cid,
                                        const estats_connection* _conn);
estats_error* estats_connection_get_addrtype(ESTATS_ADDRTYPE* _addrtype,
                                        const estats_connection* _conn);

estats_error* estats_connection_get_connection_spec(struct estats_connection_spec* _spec,
                                         const estats_connection* _conn);

estats_error* estats_connection_read_access(const estats_connection* _conn,
                                            int mode);

estats_error* estats_connection_read_value(estats_value** _value,
                                           const estats_connection* _conn,
                                           const estats_var* _var);
estats_error* estats_connection_write_value(const estats_value* _value,
                                            const estats_connection* _conn,
                                            const estats_var* _var);

estats_error* estats_connection_spec_compare(int* _result,
                 const struct estats_connection_spec* _s1,
                 const struct estats_connection_spec* _s2);

estats_error* estats_connection_spec_copy(struct estats_connection_spec* _s1,
                                    const struct estats_connection_spec* _s2);

estats_connection* estats_connection_return_next(const estats_connection* _prev);

#define ESTATS_CONNECTION_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_connection_return_next(pos))

#endif /* !defined(ESTATS_CONNECTION_H) */
