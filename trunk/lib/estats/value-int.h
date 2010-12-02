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
