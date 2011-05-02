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
#ifndef ESTATS_AGENT_H
#define ESTATS_AGENT_H

estats_error* estats_agent_attach( estats_agent **,
                             const ESTATS_AGENT_TYPE,
                             const void * /* data */);
                                
void          estats_agent_detach( estats_agent **);

estats_error* estats_agent_find_connection_from_cid( estats_connection **,
                                                     estats_agent *,
                                                     int /* cid */);

estats_error* estats_agent_find_connection_from_spec( estats_connection **,
                                                      estats_agent *,
                                         const struct estats_connection_spec *);

estats_error* estats_agent_find_connection_from_socket( estats_connection **,
                                                        estats_agent *,
                                                        int /* fd */);

estats_error* estats_agent_get_connection_head( estats_connection **,
                                                estats_agent *);

estats_error* estats_agent_get_var_head( estats_var **,
                                   const estats_agent *);

estats_error* estats_agent_find_var_from_name( estats_var **,
                                         const estats_agent *,
                                         const char * /* name */);

#endif /* ESTATS_AGENT_H */
