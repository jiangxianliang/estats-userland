#if !defined(ESTATS_STAT_H)
#define ESTATS_STAT_H

typedef enum {
    STAT_ALL     = 0,
    STAT_CID     = 1 << 0,
    STAT_PID     = 1 << 1,
    STAT_UID     = 1 << 2,
    STAT_CMD     = 1 << 3,
    STAT_SRCPORT = 1 << 4,
    STAT_DSTADDR = 1 << 5,
    STAT_DSTPORT = 1 << 6
} StatMask;

typedef struct estats_stat estats_stat;

typedef ESTATS_FOREACH_RET (*estats_stat_foreach_func)(estats_stat* stat, int flags, void* userData);

estats_error* estats_stat_head(estats_stat** _head, estats_agent* _agent); 

estats_error* estats_stat_next(estats_stat** _next, const estats_stat* _prev);

void estats_stat_free(estats_stat** _stat);

estats_error* estats_stat_foreach(estats_stat* _stat,
                                  estats_stat_foreach_func _f,
                                  void* _userData);

estats_error* estats_stat_get_cid(int* _cid,
                                  const estats_stat* _stat);

estats_error* estats_stat_get_pid(int* _pid,
                                  const estats_stat* _stat);

estats_error* estats_stat_get_uid(int* _uid,
                                  const estats_stat* _stat);

estats_error* estats_stat_get_state(int* _state,
                                    const estats_stat* _stat);

estats_error* estats_stat_get_cmdline(char** _cmdline,
                                      const estats_stat* _stat);

estats_error* estats_stat_get_spec(struct estats_connection_spec* _spec,
                                   const estats_stat* _stat);

estats_error* estats_stat_head_matched(estats_stat** _head,
	                       estats_agent* _agent,
	                       StatMask _mask,
	                       const int _cid,
			       const int _pid,
			       const int _uid,
			       const char* _cmdline,
			       const estats_value* _srcport,
			       const estats_value* _dstaddr,
			       const estats_value* _dstport);

#define estats_stat_from_cid(head, agent, cid) \
    estats_stat_head_matched(head, agent, STAT_FIND_CID, cid, 0, 0, 0)
#define estats_stat_from_pid(head, agent, pid) \
    estats_stat_head_matched(head, agent, STAT_FIND_PID, 0, pid, 0, 0)
#define estats_stat_from_uid(head, agent, uid) \
    estats_stat_head_matched(head, agent, STAT_FIND_UID, 0, 0, uid, 0)

#endif /* !defined(ESTATS_STAT_H) */
