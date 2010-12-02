#if !defined(ESTATS_CONNECTION_H)
#define ESTATS_CONNECTION_H

estats_error* estats_connection_next(estats_connection** _next,
                                     const estats_connection* _prev);

estats_error* estats_connection_get_agent(estats_agent** _agent,
                                          const estats_connection* _conn);
estats_error* estats_connection_get_cid(int* _cid,
                                        const estats_connection* _conn);
estats_error* estats_connection_get_addrtype(ESTATS_ADDRTYPE* _addrtype,
                                        const estats_connection* _conn);

estats_error* estats_connection_get_spec(struct estats_connection_spec* _spec,
                                         const estats_connection* _conn);

estats_error* estats_connection_group_access(const estats_connection* _conn,
                                             const estats_group* _group,
                                             int mode);

estats_error* estats_connection_read_value(estats_value** _value,
                                           const estats_connection* _conn,
                                           const estats_var* _var);
estats_error* estats_connection_write_value(const estats_value* _value,
                                            const estats_connection* _conn,
                                            const estats_var* _var);

estats_error* estats_connection_spec_compare(int* _result,
                 const struct estats_connection_spec* _s1,
                 const struct estats_connection_spec* _s2);

estats_error* estats_connection_spec_copy(struct estats_connection_spec* _s1,
                                    const struct estats_connection_spec* _s2);


#endif /* !defined(ESTATS_CONNECTION_H) */
