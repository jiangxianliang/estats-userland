#if !defined(ESTATS_VAR_H)
#define ESTATS_VAR_H

estats_error* estats_var_next(estats_var** _next,
                              estats_var* _prev);

estats_error* estats_var_get_name(const char** _name,
                                  const estats_var* _var);
estats_error* estats_var_get_type(ESTATS_TYPE* _type,
                                  const estats_var* _var);
estats_error* estats_var_get_size(size_t* _size,
                                  const estats_var* _var);

estats_var* estats_var_next_utility(const estats_var* prev);

#define ESTATS_VAR_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_var_next_utility(pos))

#endif /* !defined(ESTATS_VAR_H) */
