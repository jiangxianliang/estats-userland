
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
#if !defined(ESTATS_GROUP_H)
#define ESTATS_GROUP_H

estats_error* estats_group_next(estats_group** _next,
                                const estats_group* _prev);

estats_error* estats_group_get_var_head(estats_var** _var,
                                        estats_group* _group);
estats_error* estats_group_get_agent(estats_agent** _agent,
                                     const estats_group* _group);
estats_error* estats_group_get_name(const char** _name,
                                    const estats_group* _group);
estats_error* estats_group_get_size(int* _size,
                                    const estats_group* _group);
estats_error* estats_group_get_nvars(int* _nvars,
                                     const estats_group* _group);

estats_error* estats_group_find_var_from_name(estats_var** _var,
                                              const estats_group* _group,
                                              const char* _name);

estats_group* estats_group_next_utility(const estats_group* prev);

#define ESTATS_GROUP_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_group_next_utility(pos))

#endif /* !defined(ESTATS_GROUP_H) */
