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

#include <endian.h>
#include <byteswap.h>

# if __BYTE_ORDER == __LITTLE_ENDIAN
#  define htolog16(x) (x)
#  define log16toh(x) (x)

#  define htolog32(x) (x)
#  define log32toh(x) (x)

#  define htolog64(x) (x)
#  define log64toh(x) (x)
# else
#  define htolog16(x) bswap_16 (x)
#  define log16toh(x) bswap_16 (x)

#  define htolog32(x) bswap_32 (x)
#  define log32toh(x) bswap_32 (x)

#  define htolog64(x) bswap_64 (x)
#  define log64toh(x) bswap_64 (x)
# endif

#endif /* !defined(ESTATS_LOG_INT_H) */
