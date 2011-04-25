
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
#if !defined(ESTATS_VAR_INT_H)
#define ESTATS_VAR_INT_H

#ifdef PRINT_DEPRECATED_WARNINGS
# define CHECK_VAR(x) _estats_var_dep_check(x)
#else
# define CHECK_VAR(x)
#endif

estats_error* _estats_var_size_from_type(int* size, ESTATS_TYPE type);
void          _estats_var_dep_check(estats_var* var);
estats_error* _estats_var_next_undeprecated(estats_var** next, const estats_var* prev);

#endif /* !defined(ESTATS_VAR_INT_H) */
