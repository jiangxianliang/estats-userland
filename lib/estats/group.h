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
/*
estats_error* estats_group_foreach_var(estats_group* _group,
                                       estats_var_foreach_func _f,
                                       void* _userData);
*/
estats_error* estats_group_find_var_from_name(estats_var** _var,
                                              const estats_group* _group,
                                              const char* _name);

estats_group* estats_group_next_utility(const estats_group* prev);

#define ESTATS_GROUP_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = estats_group_next_utility(pos))

#endif /* !defined(ESTATS_GROUP_H) */
