#if !defined(ESTATS_CONNLIST_INT_H)
#define ESTATS_CONNLIST_INT_H

#include <sys/stat.h>
#include <fnmatch.h>

typedef enum CONNLIST_LIST_POS {
    CONNLIST_IS_HEAD = 0,
    CONNLIST_IS_NOT_HEAD
} CONNLIST_LIST_POS;

struct estats_connlist {
  struct estats_list            list; 
  CONNLIST_LIST_POS		pos;
  int                           cid;
  pid_t                         pid;
  uid_t                         uid;
  ino_t                         ino;
  int                           state;
  char*                         cmdline;
  ESTATS_ADDRTYPE               addrtype;
  struct estats_connection_spec spec; 
};

#endif /* !defined(ESTATS_CONNLIST_INT_H) */
