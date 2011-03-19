#if !defined(ESTATS_H)
#define ESTATS_H

/* TODO: How to handle systems without the following headers? */
/* TODO: answer---add check to configure */
#include <sys/types.h>  /* For size_t */
#include <netinet/in.h> /* For struct in_addr, in6_addr */
#include <inttypes.h>   /* For uint8_t and friends, C99 standard header */
#include <unistd.h>     /* For R_OK, W_OK, ... */
#include <stdio.h>      /* For FILE* */
#include <stdlib.h>     /* For NULL */

#include <estats/types.h> /* Must be first */
#include <estats/agent.h>
#include <estats/connection.h>
#include <estats/error.h>
#include <estats/group.h>
#include <estats/log.h>
#include <estats/snapshot.h>
#include <estats/conninfo.h>
#include <estats/value.h>
#include <estats/var.h>

#endif /* !defined(ESTATS_H) */
