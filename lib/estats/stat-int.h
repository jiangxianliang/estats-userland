#if !defined(ESTATS_STAT_INT_H)
#define ESTATS_STAT_INT_H

#include <sys/stat.h>
#include <fnmatch.h>

typedef enum STAT_LIST_POS {
    STAT_IS_HEAD = 0,
    STAT_IS_NOT_HEAD
} STAT_LIST_POS;

struct estats_stat {
  struct estats_list            list; 
  STAT_LIST_POS			pos;
  int                           cid;
  pid_t                         pid;
  uid_t                         uid;
  ino_t                         ino;
  int                           state;
  char*                         cmdline;
  ESTATS_ADDRTYPE               addrtype;
  struct estats_connection_spec spec; 
};

#endif /* !defined(ESTATS_STAT_INT_H) */
