#if !defined(ESTATS_LOG_H)
#define ESTATS_LOG_H

estats_error* estats_log_open(estats_log** _log, const char* _path, const char* _mode);
void          estats_log_close(estats_log** _log);

estats_error* estats_log_write(estats_log* _log, const estats_snapshot* _snap);

estats_error* estats_log_snapshot_alloc(estats_snapshot** _snap, estats_log* _log);
estats_error* estats_log_snap(estats_snapshot* _snap, estats_log* _log);

estats_error* estats_log_get_agent(estats_agent** _agent, const estats_log* _log);

estats_error* estats_log_get_mode(int* mode, const estats_log* _log);

estats_error* estats_log_get_spec(struct estats_connection_spec* _spec, const estats_log* _log);
estats_error* estats_log_get_annotation(char** _ann, const estats_log* _log);
estats_error* estats_log_set_annotation(estats_log* _log, const char* _ann);
estats_error* estats_log_eof(int* _eof, const estats_log* _log);

#endif /* !defined(ESTATS_LOG_H) */
