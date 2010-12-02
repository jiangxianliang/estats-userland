#if !defined(ESTATS_SNAPSHOT_H)
#define ESTATS_SNAPSHOT_H

estats_error* estats_snapshot_alloc(estats_snapshot** _snap,
                                    estats_group* _group,
                                    estats_connection* _conn);
void          estats_snapshot_free(estats_snapshot** _snap);

estats_error* estats_snapshot_get_group(estats_group** _group,
                                        const estats_snapshot* _snap);
estats_error* estats_snapshot_get_group_name(const char** _name,
                                             const estats_snapshot* _snap);

estats_error* estats_snap(estats_snapshot* _snap);

estats_error* estats_snapshot_read_value(estats_value** _value,
                                         const estats_snapshot* _snap,
                                         const estats_var* _var);
estats_error* estats_snapshot_delta(estats_value** _value,
                                    const estats_snapshot* _s1,
                                    const estats_snapshot* _s2,
                                    const estats_var* _var);

#endif /* !defined(ESTATS_SNAPSHOT_H) */
