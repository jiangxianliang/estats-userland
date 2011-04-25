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
#if !defined(ESTATS_AGENT_H)
#define ESTATS_AGENT_H

estats_error* estats_agent_attach(estats_agent** _agent,
                                  const ESTATS_AGENT_TYPE _type,
                                  const void* _data);
void          estats_agent_detach(estats_agent** _agent);

estats_error* estats_agent_get_connection_head(estats_connection** _conn,
                                               estats_agent* _agent);
estats_error* estats_agent_get_group_head(estats_group** _group,
                                          estats_agent* _agent);
estats_error* estats_agent_get_type(ESTATS_AGENT_TYPE* _type,
                                    const estats_agent* _agent);
estats_error* estats_agent_get_version(const char** _version,
                                       const estats_agent* _agent);

estats_error* estats_agent_find_connection_from_cid(estats_connection** _conn,
                                                    estats_agent* _agent,
                                                    int _cid);
estats_error* estats_agent_find_connection_from_spec(estats_connection** _conn,
                                                     estats_agent* _agent,
                                                     const struct estats_connection_spec* _spec);

estats_error* estats_agent_find_connection_from_socket(estats_connection** _conn,
                                                       estats_agent* _agent,
                                                       int _sockfd);

estats_error* estats_agent_find_group_from_name(estats_group** _group,
                                                const estats_agent* _agent,
                                                const char* _name);

estats_error* estats_agent_find_var_and_group(estats_var** _var,
                                              estats_group** _group,
                                              estats_agent* _agent,
                                              const char* _varname);

#endif /* !defined(ESTATS_AGENT_H) */
