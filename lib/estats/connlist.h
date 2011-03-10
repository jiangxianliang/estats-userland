#if !defined(ESTATS_CONNLIST_H)
#define ESTATS_CONNLIST_H

typedef enum {
    CONNLIST_ALL     = 0,
    CONNLIST_CID     = 1 << 0,
    CONNLIST_PID     = 1 << 1,
    CONNLIST_UID     = 1 << 2,
    CONNLIST_CMD     = 1 << 3,
    CONNLIST_SRCPORT = 1 << 4,
    CONNLIST_DSTADDR = 1 << 5,
    CONNLIST_DSTPORT = 1 << 6
} ConnlistMask;

typedef struct estats_connlist estats_connlist;

typedef ESTATS_FOREACH_RET (*estats_connlist_foreach_func)(estats_connlist* connlist, int flags, void* userData);

estats_error* estats_connlist_head(estats_connlist** _head, estats_agent* _agent); 

estats_error* estats_connlist_next(estats_connlist** _next, const estats_connlist* _prev);

void estats_connlist_free(estats_connlist** _connlist);

estats_error* estats_connlist_foreach(estats_connlist* _connlist,
                                  estats_connlist_foreach_func _f,
                                  void* _userData);

estats_error* estats_connlist_get_cid(int* _cid,
                                  const estats_connlist* _connlist);

estats_error* estats_connlist_get_pid(int* _pid,
                                  const estats_connlist* _connlist);

estats_error* estats_connlist_get_uid(int* _uid,
                                  const estats_connlist* _connlist);

estats_error* estats_connlist_get_connliste(int* _connliste,
                                    const estats_connlist* _connlist);

estats_error* estats_connlist_get_cmdline(char** _cmdline,
                                      const estats_connlist* _connlist);

estats_error* estats_connlist_get_spec(struct estats_connection_spec* _spec,
                                   const estats_connlist* _connlist);

estats_error* estats_connlist_head_matched(estats_connlist** _head,
	                       estats_agent* _agent,
	                       ConnlistMask _mask,
	                       const int _cid,
			       const int _pid,
			       const int _uid,
			       const char* _cmdline,
			       const estats_value* _srcport,
			       const estats_value* _dstaddr,
			       const estats_value* _dstport);

#define estats_connlist_from_cid(head, agent, cid) \
    estats_connlist_head_matched(head, agent, CONNLIST_FIND_CID, cid, 0, 0, 0)
#define estats_connlist_from_pid(head, agent, pid) \
    estats_connlist_head_matched(head, agent, CONNLIST_FIND_PID, 0, pid, 0, 0)
#define estats_connlist_from_uid(head, agent, uid) \
    estats_connlist_head_matched(head, agent, CONNLIST_FIND_UID, 0, 0, uid, 0)

#endif /* !defined(ESTATS_CONNLIST_H) */
