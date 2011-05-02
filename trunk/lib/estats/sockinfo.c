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
#include <estats/estats-int.h>
#include <estats/sockinfo.h>

static estats_error* _estats_sockinfo_refresh(estats_sockinfo** _sockinfo, estats_agent* agent);
static estats_error* _estats_sockinfo_get_tcp_list(struct estats_sockinfo** tcp_list, estats_agent* agent);
static estats_error* _estats_sockinfo_get_ino_list(struct estats_sockinfo** ino_list);
static estats_error* _estats_sockinfo_get_pid_list(struct estats_sockinfo** pid_list);
static estats_error* _estats_sockinfo_new_node(estats_sockinfo** _sockinfo);
static void          _estats_sockinfo_free_node(estats_sockinfo** sockinfo);
static estats_error* _estats_sockinfo_add_tail(struct estats_sockinfo** head, struct estats_sockinfo* ci);


estats_error*
estats_get_sockinfo_head(estats_sockinfo** head, estats_agent* agent)
{
    estats_error* err = NULL;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_sockinfo_refresh(head, agent));

Cleanup:
    return err;
}

static estats_error*
_estats_sockinfo_new_node(estats_sockinfo** ci)
{
    estats_error* err = NULL; 

    ErrIf(ci == NULL, ESTATS_ERR_INVAL);                                               
    *ci = NULL;

    Chk(Malloc((void**) ci, sizeof(estats_sockinfo)));
    memset((void*) *ci, 0, sizeof(estats_sockinfo));

Cleanup:
    return err;
}

estats_error*
estats_sockinfo_next(estats_sockinfo** next, const estats_sockinfo* prev)
{
    estats_error* err = NULL;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    (*next) = prev->next;

Cleanup:
    return err;
}

estats_sockinfo*
estats_sockinfo_return_next(const estats_sockinfo* prev)
{
    estats_error* err = NULL;
    estats_sockinfo* next = NULL;

    Chk(estats_sockinfo_next(&next, prev));

Cleanup:
    if (err) return NULL;
    else return next;
}

void
estats_sockinfo_free(estats_sockinfo** sockinfo)
{
    struct estats_sockinfo* tmp;

    if (sockinfo == NULL || *sockinfo == NULL)
        return;

    while (*sockinfo != NULL) {
        tmp = *sockinfo;
        *sockinfo = (*sockinfo)->next;
        _estats_sockinfo_free_node(&tmp);
    }
}

void
_estats_sockinfo_free_node(estats_sockinfo** sockinfo)
{
    if (sockinfo == NULL || *sockinfo == NULL)
	return;

    Free((void**) sockinfo);
}

estats_error*
estats_sockinfo_get_cid(int* cid, const estats_sockinfo* ecl)
{
    estats_error* err = NULL;

    ErrIf(cid == NULL || ecl == NULL, ESTATS_ERR_INVAL);
    *cid = ecl->cid;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_pid(int* pid, const estats_sockinfo* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(pid == NULL || sockinfo == NULL, ESTATS_ERR_INVAL);
    *pid = sockinfo->pid;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_uid(int* uid, const estats_sockinfo* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(uid == NULL || sockinfo == NULL, ESTATS_ERR_INVAL);
    *uid = sockinfo->uid;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_state(int* state, const estats_sockinfo* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(state == NULL || sockinfo == NULL, ESTATS_ERR_INVAL);
    *state = sockinfo->state;

Cleanup:
    return err;
}


estats_error*
estats_sockinfo_get_cmdline(char** str, const estats_sockinfo* sockinfo)
{
    estats_error* err = NULL;

    ErrIf(sockinfo == NULL, ESTATS_ERR_INVAL);
    Chk(Strdup(str, sockinfo->cmdline));

Cleanup:
    return err;
}


static estats_error*
_estats_sockinfo_refresh(struct estats_sockinfo** ecl, estats_agent* agent)
{
    estats_error* err = NULL;
    estats_sockinfo* tcp_head = NULL;
    estats_sockinfo* ino_head = NULL;
    estats_sockinfo* pid_head = NULL;
    estats_sockinfo* tcp_pos;
    estats_sockinfo* ino_pos;
    estats_sockinfo* pid_pos;
    estats_sockinfo* newcl;
    int dif;
    int tcp_entry, fd_entry;

    /* associate cid with address */ 
    Chk(_estats_sockinfo_get_tcp_list(&tcp_head, agent));

    /* associate address with ino */
    Chk(_estats_sockinfo_get_ino_list(&ino_head));

    /* associate ino with pid */
    Chk(_estats_sockinfo_get_pid_list(&pid_head));

    /* collate above */

    ESTATS_SOCKINFO_FOREACH(tcp_pos, tcp_head) { 

	tcp_entry = 0;

        ESTATS_SOCKINFO_FOREACH(ino_pos, ino_head) {

	    Chk(estats_connection_spec_compare(&dif, &ino_pos->spec, &tcp_pos->spec));

	    if (!dif) {
	       	tcp_entry = 1;
	       	fd_entry = 0;

                ESTATS_SOCKINFO_FOREACH(pid_pos, pid_head) {

		    if(pid_pos->ino == ino_pos->ino) { //then create entry 
			fd_entry = 1;

                        Chk(_estats_sockinfo_new_node(&newcl));

			newcl->pid = pid_pos->pid; 

                        strlcpy(newcl->cmdline, pid_pos->cmdline, sizeof(pid_pos->cmdline));
                        newcl->uid = ino_pos->uid;
		       	newcl->state = ino_pos->state;

			newcl->cid = tcp_pos->cid;

			newcl->addrtype = tcp_pos->addrtype; 

                        newcl->spec = tcp_pos->spec; // struct copy

                        Chk(_estats_sockinfo_add_tail(ecl, newcl));
		    }
	       	}
	       	if(!fd_entry) { // add entry w/out cmdline 

                        Chk(_estats_sockinfo_new_node(&newcl));

			newcl->pid = 0;
			newcl->uid = ino_pos->uid;
		       	newcl->state = ino_pos->state;

			newcl->cid = tcp_pos->cid;
			newcl->addrtype = tcp_pos->addrtype; 

                        newcl->spec = tcp_pos->spec; // struct copy

                        strlcpy(newcl->cmdline, "\0", 1);
                        Chk(_estats_sockinfo_add_tail(ecl, newcl));
	       	}
	    }
	}
	if(!tcp_entry) { // then connection has vanished; add residual cid info
			 // (only for consistency with entries in /proc/web100) 

            Chk(_estats_sockinfo_new_node(&newcl));

	    newcl->cid = tcp_pos->cid; 
	    newcl->addrtype = tcp_pos->addrtype; 

            newcl->spec = tcp_pos->spec; // struct copy

            strlcpy(newcl->cmdline, "\0", 1);

            Chk(_estats_sockinfo_add_tail(ecl, newcl));
       	}
    }

Cleanup:
    estats_sockinfo_free(&tcp_head);
    estats_sockinfo_free(&ino_head);
    estats_sockinfo_free(&pid_head);

    return err;
}


static estats_error*
_estats_sockinfo_get_tcp_list(struct estats_sockinfo** head, estats_agent* agent)
{
    estats_error* err = NULL; 
    estats_connection* conn;

    ErrIf(head == NULL, ESTATS_ERR_INVAL);
    *head = NULL;

    Chk(estats_agent_get_connection_head(&conn, agent));

    while (conn != NULL) { 
	estats_sockinfo* sockinfo = NULL; 

        Chk(_estats_sockinfo_new_node(&sockinfo));

	Chk(estats_connection_get_cid(&sockinfo->cid, conn));
       	Chk(estats_connection_get_connection_spec(&sockinfo->spec, conn));
	Chk(estats_connection_get_addrtype(&sockinfo->addrtype, conn));

        Chk(_estats_sockinfo_add_tail(head, sockinfo));

	Chk(estats_connection_next(&conn, conn));
    }

Cleanup:
    return err;
}


static estats_error*
_estats_sockinfo_get_ino_list(struct estats_sockinfo** head)
{ 
    estats_error* err = NULL;
    FILE *file = NULL;
    FILE *file6 = NULL;
    char buf[256];
    int scan;
    struct in6_addr in6;

    ErrIf(head == NULL, ESTATS_ERR_INVAL);
    *head = NULL;

    file = fopen("/proc/net/tcp", "r");
    file6 = fopen("/proc/net/tcp6", "r");

    if (file) {
	estats_sockinfo* sockinfo;
	uint32_t srcAddr;
	uint16_t srcPort;
	uint32_t dstAddr;
	uint16_t dstPort;

       	while (fgets(buf, sizeof(buf), file) != NULL) {

	    Chk(_estats_sockinfo_new_node(&sockinfo));

	    if ((scan = sscanf(buf,
			    "%*u: %x:%hx %x:%hx %x %*x:%*x %*x:%*x %*x %u %*u %lu",
			    (uint32_t *) &(sockinfo->spec.src_addr),
			    (uint16_t *) &(sockinfo->spec.src_port),
			    (uint32_t *) &(sockinfo->spec.dst_addr),
			    (uint16_t *) &(sockinfo->spec.dst_port),
			    (int *) &(sockinfo->state),
			    (uid_t *) &(sockinfo->uid),
			    (ino_t *) &(sockinfo->ino))) == 7) { 

		sockinfo->addrtype = ESTATS_ADDRTYPE_IPV4;

                Chk(_estats_sockinfo_add_tail(head, sockinfo));
	    } else {
	       	free(sockinfo);
	    }
       	}
       	fclose(file);
    }

    if (file6) { 
	estats_sockinfo* sockinfo;
	char srcAddr[INET6_ADDRSTRLEN];
	uint16_t srcPort;
	char dstAddr[INET6_ADDRSTRLEN];
	uint16_t dstPort;

	while (fgets(buf, sizeof(buf), file6) != NULL) {

	    Chk(_estats_sockinfo_new_node(&sockinfo));

	    if ((scan = sscanf(buf,
			    "%*u: %64[0-9A-Fa-f]:%hx %64[0-9A-Fa-f]:%hx %x %*x:%*x %*x:%*x %*x %u %*u %u", 
			    (char *) &srcAddr,
			    (uint16_t *) &(sockinfo->spec.src_port),
			    (char *) &dstAddr,
			    (uint16_t *) &(sockinfo->spec.dst_port),
			    (int *) &(sockinfo->state),
			    (uid_t *) &(sockinfo->uid),
			    (pid_t *) &(sockinfo->ino))) == 7) { 

		sscanf(srcAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]); 

                memcpy(&(sockinfo->spec.src_addr), &in6.s6_addr, 16);

		sscanf(dstAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]);

                memcpy(&(sockinfo->spec.dst_addr), &in6.s6_addr, 16);

		sockinfo->addrtype = ESTATS_ADDRTYPE_IPV6;

                Chk(_estats_sockinfo_add_tail(head, sockinfo));
	    } else { 
		free(sockinfo);
	    }
       	}
       	fclose(file6);
    } 

Cleanup:
    return err;
}


static estats_error*
_estats_sockinfo_get_pid_list(struct estats_sockinfo** head)
{ 
    estats_error* err;
    estats_sockinfo* ecl;
    DIR *dir, *fddir; 
    struct dirent *direntp, *fddirentp;
    pid_t pid;
    char path[PATH_MAX];
    char buf[256];
    struct stat st;
    int stno;
    FILE* file;

    ErrIf(head == NULL, ESTATS_ERR_INVAL);
    *head = NULL;

    Chk(Opendir(&dir, "/proc"));

    while ((direntp = readdir(dir)) != NULL) {
       	if ((pid = atoi(direntp->d_name)) != 0)
       	{
	    sprintf(path, "%s/%d/%s/", "/proc", pid, "fd"); 
	    if ((fddir = opendir(path)) != NULL)  //else lacks permissions 
	    { 
		while ((fddirentp = readdir(fddir)) != NULL) 
		{ 
		    strcpy(buf, path);
		    strcat(buf, fddirentp->d_name); 
		    stno = stat(buf, &st); 
		    if (S_ISSOCK(st.st_mode)) { // add new list entry

			sprintf(buf, "/proc/%d/status", pid);

			if ((file = fopen(buf, "r")) == NULL) continue;

			if (fgets(buf, sizeof(buf), file) == NULL)
			    goto FileCleanup; 
       	
	                Chk(_estats_sockinfo_new_node(&ecl));

			if (sscanf(buf, "Name: %16s\n", ecl->cmdline) != 1) {
			    Free((void**) &ecl);
			    goto FileCleanup;
		       	}

			ecl->ino = st.st_ino;
		       	ecl->pid = pid;
                        Chk(_estats_sockinfo_add_tail(head, ecl));

FileCleanup:
			fclose(file); 
		    }
	       	}
	       	closedir(fddir);
	    }
       	}
    }
    closedir(dir);

Cleanup:
    return err;
}

static estats_error*
_estats_sockinfo_add_tail(struct estats_sockinfo** head, struct estats_sockinfo* ci)
{
    estats_error* err = NULL;

    ErrIf(ci->next != NULL, ESTATS_ERR_INVAL);

    ci->next = *head;
    *head = ci;

Cleanup:
    return err;
}
