#if !defined(ESTATS_DEBUG_INT_H)
#define ESTATS_DEBUG_INT_H

#if !defined(IFDEBUG)
# if defined(DEBUG)
#  define IFDEBUG(x) (x)
# else
#  define IFDEBUG(x)
# endif /* defined(DEBUG) */
#endif /* !defined(IFDEBUG) */

#if defined(DEBUG)
#define dbgprintf(fmt, args...) \
    do { \
        dbgprintf_no_prefix("DBG: "); \
        dbgprintf_no_prefix(fmt, ## args); \
    } while (0)
#else
# define dbgprintf(fmt, args...)
#endif

void dbgprintf_no_prefix(const char* _fmt, ...);

#endif /* !defined(ESTATS_DEBUG_INT_H) */
