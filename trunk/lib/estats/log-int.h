#if !defined(ESTATS_LOG_INT_H)
#define ESTATS_LOG_INT_H

#include <sys/stat.h>

typedef enum LOG_MODE {
    LOG_READ = 0,
    LOG_WRITE = 1
} LOG_MODE;

struct estats_log {
    FILE*                          fp; 
    LOG_MODE                       mode;
    int                            first_write; 
    struct estats_agent*           agent;
    struct estats_connection_spec  spec;
    char*                          note;
};

#endif /* !defined(ESTATS_LOG_INT_H) */
