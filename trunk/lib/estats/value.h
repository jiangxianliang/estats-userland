
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
#if !defined(ESTATS_VALUE_H)
#define ESTATS_VALUE_H

estats_error* estats_value_new(estats_value** _value,
                               const void* buf,
                               ESTATS_VALUE_TYPE _type);
void          estats_value_free(estats_value** _value);

estats_error* estats_value_get_type(ESTATS_VALUE_TYPE* _type,
                                    const estats_value* _value);

estats_error* estats_value_change_type(estats_value** _value,
                                       ESTATS_VALUE_TYPE _newType);

estats_error* estats_value_as_uint16(uint16_t* _u16,
                                     const estats_value* _value);
estats_error* estats_value_as_int32(int32_t* _s32,
                                    const estats_value* _value);
estats_error* estats_value_as_uint32(uint32_t* _u32,
                                     const estats_value* _value);
estats_error* estats_value_as_uint64(uint64_t* _u64,
                                     const estats_value* _value);
estats_error* estats_value_as_string(char** _str,
                                     const estats_value* _value);
estats_error* estats_value_as_ip4addr(struct in_addr* _ip4addr,
                                      const estats_value* _value);
estats_error* estats_value_as_ip6addr(struct in6_addr* _ip6addr,
                                      const estats_value* _value);

estats_error* estats_value_compare(int* _result,
	                           const estats_value* _v1,
			           const estats_value* _v2);

estats_error* estats_value_difference(estats_value** _result,
                                      const estats_value* _v1,
                                      const estats_value* _v2);

#endif /* !defined(ESTATS_VALUE_H) */
