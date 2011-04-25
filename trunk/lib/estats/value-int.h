
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
#if !defined(ESTATS_VALUE_INT_H)
#define ESTATS_VALUE_INT_H

estats_error* _estats_value_from_var_buf(estats_value** _value,
                                         const void* _buf,
                                         ESTATS_TYPE _type);
estats_error* _estats_value_to_var_buf(void** _buf,
                                       size_t* _size,
                                       const estats_value* _value,
                                       ESTATS_TYPE _type);
estats_error* _estats_value_copy(estats_value** copyVal,
	                         const estats_value* origVal);

#endif /* !defined(ESTATS_VALUE_INT_H) */
