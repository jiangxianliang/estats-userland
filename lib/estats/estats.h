
/*
 * Copyright (c) 2011 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */
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
#include <estats/conninfo.h>
#include <estats/error.h>
#include <estats/group.h>
#include <estats/log.h>
#include <estats/snapshot.h>
#include <estats/value.h>
#include <estats/var.h>

#endif /* !defined(ESTATS_H) */
