#if !defined(ESTATS_CONNINFO_H)
#define ESTATS_CONNINFO_H

typedef struct estats_conninfo {
    int                            cid;
    pid_t                          pid;
    uid_t                          uid;
    ino_t                          ino;
    int                            state;
    char*                          cmdline;
    ESTATS_ADDRTYPE                addrtype;
    struct estats_connection_spec  spec;
    struct estats_conninfo        *next;
} estats_conninfo;

estats_error* estats_get_conninfo_head(estats_conninfo** _head, estats_agent* _agent);

//estats_error* estats_conninfo_new(estats_conninfo** _conninfo);

estats_error* estats_conninfo_next(estats_conninfo** _next, const estats_conninfo* _prev);

void          estats_conninfo_free(estats_conninfo** _conninfo);

estats_error* estats_conninfo_get_cid(int* _cid,
                                  const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_pid(int* _pid,
                                  const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_uid(int* _uid,
                                  const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_state(int* _state,
                                    const estats_conninfo* _conninfo);

estats_error* estats_conninfo_get_cmdline(char** _cmdline,
                                      const estats_conninfo* _conninfo);

estats_error* estats_conninfo_copy_spec(struct estats_connection_spec* _spec,
                                   const estats_conninfo* _conninfo);

#define ESTATS_CONNINFO_FOREACH(pos, head) \
    for (pos = head; pos != NULL; pos = pos->next)

#endif /* !defined(ESTATS_CONNINFO_H) */
