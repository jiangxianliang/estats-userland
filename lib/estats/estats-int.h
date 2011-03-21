#if !defined(ESTATS_INT_H)
#define ESTATS_INT_H

/* System headers (use config.h's results to determine whether or not to
   include them) */
#include "config.h"
#include <sys/param.h> /* For PATH_MAX */
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> /* For inet_ntop */
#include <netinet/in.h> /* For ntohs and friends */
#include <ctype.h> /* For isspace() */
#include <dirent.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define ARRAYSIZE(x) (sizeof(x) / sizeof(x[0]))

/* Compat headers */
#if defined(USE_LOCAL_STRLCPY)
#include "strlcpy.h"
#endif

#include <estats/types.h>
#include <estats/types-int.h>

/* Estats internal headers */
#include <estats/agent-int.h>
#include <estats/debug-int.h>
#include <estats/error-int.h>
#include <estats/libcwrap-int.h>
#include <estats/list-int.h>
#include <estats/log-int.h>
#include <estats/string-int.h>
#include <estats/value-int.h>
#include <estats/var-int.h>

/* Estats public headers */
#include <estats/agent.h>
#include <estats/connection.h>
#include <estats/error.h>
#include <estats/group.h>
#include <estats/log.h>
#include <estats/snapshot.h>
#include <estats/value.h>
#include <estats/var.h>

#endif /* !defined(ESTATS_INT_H) */
