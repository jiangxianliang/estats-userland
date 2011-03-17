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
