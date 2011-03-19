#include <estats/estats-int.h>
#include <estats/conninfo.h>

static estats_error* _estats_conninfo_refresh(estats_conninfo** _conninfo, estats_agent* agent);
static estats_error* _estats_conninfo_get_tcp_list(struct estats_conninfo** tcp_list, estats_agent* agent);
static estats_error* _estats_conninfo_get_ino_list(struct estats_conninfo** ino_list);
static estats_error* _estats_conninfo_get_pid_list(struct estats_conninfo** pid_list);
static estats_error* _estats_conninfo_spec_compare(int* res, const estats_conninfo* st1, const estats_conninfo* st2);
static estats_error* _estats_conninfo_new_node(estats_conninfo** _conninfo);
static void          _estats_conninfo_free_node(estats_conninfo** conninfo);
static estats_error* _estats_conninfo_add_tail(struct estats_conninfo** head, struct estats_conninfo* ci);

estats_error*
estats_get_conninfo_head(estats_conninfo** head, estats_agent* agent)
{
    estats_error* err = NULL;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_conninfo_refresh(head, agent));

Cleanup:
    return err;
}

estats_error*
_estats_conninfo_new_node(estats_conninfo** ci)
{
    estats_error* err = NULL; 

    ErrIf(ci == NULL, ESTATS_ERR_INVAL);                                               
    *ci = NULL;

    Chk(Malloc((void**) ci, sizeof(estats_conninfo)));

    (*ci)->spec.dst_port = NULL;
    (*ci)->spec.dst_addr = NULL;
    (*ci)->spec.src_port = NULL;
    (*ci)->spec.src_addr = NULL;
//    (*ci)->cmdline = NULL;
    strlcpy((*ci)->cmdline, "\0", 1);
    (*ci)->next = NULL;

Cleanup:
    return err;
}

estats_error*
estats_conninfo_next(estats_conninfo** next, const estats_conninfo* prev)
{
    estats_error* err = NULL;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    (*next) = prev->next;

Cleanup:
    return err;
}

void
estats_conninfo_free(estats_conninfo** conninfo)
{
    struct estats_conninfo* tmp;

    if (conninfo == NULL || *conninfo == NULL)
        return;

    while (*conninfo != NULL) {
        tmp = *conninfo;
        *conninfo = (*conninfo)->next;
        _estats_conninfo_free_node(&tmp);
    }
}

void
_estats_conninfo_free_node(estats_conninfo** conninfo)
{
    if (conninfo == NULL || *conninfo == NULL)
	return;

    estats_value_free(&((*conninfo)->spec.src_addr));
    estats_value_free(&((*conninfo)->spec.src_port));
    estats_value_free(&((*conninfo)->spec.dst_addr));
    estats_value_free(&((*conninfo)->spec.dst_port));
//    Free((void**) &(*conninfo)->cmdline);

    Free((void**) conninfo);
}

estats_error*
estats_conninfo_get_cid(int* cid, const estats_conninfo* ecl)
{
    estats_error* err = NULL;

    ErrIf(cid == NULL || ecl == NULL, ESTATS_ERR_INVAL);
    *cid = ecl->cid;

Cleanup:
    return err;
}


estats_error*
estats_conninfo_get_pid(int* pid, const estats_conninfo* conninfo)
{
    estats_error* err = NULL;

    ErrIf(pid == NULL || conninfo == NULL, ESTATS_ERR_INVAL);
    *pid = conninfo->pid;

Cleanup:
    return err;
}


estats_error*
estats_conninfo_get_uid(int* uid, const estats_conninfo* conninfo)
{
    estats_error* err = NULL;

    ErrIf(uid == NULL || conninfo == NULL, ESTATS_ERR_INVAL);
    *uid = conninfo->uid;

Cleanup:
    return err;
}


estats_error*
estats_conninfo_get_state(int* state, const estats_conninfo* conninfo)
{
    estats_error* err = NULL;

    ErrIf(state == NULL || conninfo == NULL, ESTATS_ERR_INVAL);
    *state = conninfo->state;

Cleanup:
    return err;
}


estats_error*
estats_conninfo_copy_spec(struct estats_connection_spec *spec, const estats_conninfo* st)
{
    estats_error* err = NULL;
    const struct estats_connection_spec* sp;

    ErrIf(spec == NULL || st == NULL, ESTATS_ERR_INVAL);
    ErrIf((sp = &st->spec) == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_value_copy(&spec->src_addr, sp->src_addr));
    Chk(_estats_value_copy(&spec->src_port, sp->src_port));
    Chk(_estats_value_copy(&spec->dst_addr, sp->dst_addr));
    Chk(_estats_value_copy(&spec->dst_port, sp->dst_port));

Cleanup:
    return err;
}


estats_error*
estats_conninfo_get_cmdline(char** str, const estats_conninfo* conninfo)
{
    estats_error* err = NULL;

    ErrIf(conninfo == NULL, ESTATS_ERR_INVAL);
    Chk(Strdup(str, conninfo->cmdline));

Cleanup:
    return err;
}


static estats_error*
_estats_conninfo_refresh(struct estats_conninfo** ecl, estats_agent* agent)
{
    estats_error* err = NULL;
    estats_conninfo* tcp_head = NULL;
    estats_conninfo* ino_head = NULL;
    estats_conninfo* pid_head = NULL;
    estats_conninfo* tcp_pos;
    estats_conninfo* ino_pos;
    estats_conninfo* pid_pos;
    estats_conninfo* newcl;
    estats_conninfo* tst_pos;
    int dif;
    int tcp_entry, fd_entry;
    int ii;

    /* associate cid with address */ 
    Chk(_estats_conninfo_get_tcp_list(&tcp_head, agent));

    /* associate address with ino */
    Chk(_estats_conninfo_get_ino_list(&ino_head));

    /* associate ino with pid */
    Chk(_estats_conninfo_get_pid_list(&pid_head));

    /* collate above */

    ESTATS_CONNINFO_FOREACH(tcp_pos, tcp_head) { 

	tcp_entry = 0;

        ESTATS_CONNINFO_FOREACH(ino_pos, ino_head) {

	    Chk(_estats_conninfo_spec_compare(&dif, ino_pos, tcp_pos));

	    if (!dif) {
	       	tcp_entry = 1;
	       	fd_entry = 0;

                ESTATS_CONNINFO_FOREACH(pid_pos, pid_head) {

		    if(pid_pos->ino == ino_pos->ino) { //then create entry 
			fd_entry = 1;

                        Chk(_estats_conninfo_new_node(&newcl));

			newcl->pid = pid_pos->pid; 
//			Chk(Strdup(&newcl->cmdline, pid_pos->cmdline));

                        strlcpy(newcl->cmdline, pid_pos->cmdline, sizeof(pid_pos->cmdline));
                        newcl->uid = ino_pos->uid;
		       	newcl->state = ino_pos->state;

			newcl->cid = tcp_pos->cid;

			newcl->addrtype = tcp_pos->addrtype; 

			Chk(estats_conninfo_copy_spec(&newcl->spec, tcp_pos));

                        Chk(_estats_conninfo_add_tail(ecl, newcl));
		    }
	       	}
	       	if(!fd_entry) { // add entry w/out cmdline 

                        Chk(_estats_conninfo_new_node(&newcl));

			newcl->pid = 0;
			newcl->uid = ino_pos->uid;
		       	newcl->state = ino_pos->state;

			newcl->cid = tcp_pos->cid;
			newcl->addrtype = tcp_pos->addrtype; 
			Chk(estats_conninfo_copy_spec(&newcl->spec, tcp_pos));

//			Chk(Strdup(&newcl->cmdline, "\0")); 

                        strlcpy(newcl->cmdline, "\0", 1);
                        Chk(_estats_conninfo_add_tail(ecl, newcl));
	       	}
	    }
	}
	if(!tcp_entry) { // then connection has vanished; add residual cid info
			 // (only for consistency with entries in /proc/web100) 

            Chk(_estats_conninfo_new_node(&newcl));

	    newcl->cid = tcp_pos->cid; 
	    newcl->addrtype = tcp_pos->addrtype; 
	    Chk(estats_conninfo_copy_spec(&newcl->spec, tcp_pos));
//	    Chk(Strdup(&newcl->cmdline, "\0")); 

            strlcpy(newcl->cmdline, "\0", 1);

            Chk(_estats_conninfo_add_tail(ecl, newcl));
       	}
    }
    ii = 0;
    ESTATS_CONNINFO_FOREACH(tcp_pos, tcp_head) {
        ii++;
    }
    printf("ii = %d\n", ii);
Cleanup:
    estats_conninfo_free(&tcp_head);
    estats_conninfo_free(&ino_head);
    estats_conninfo_free(&pid_head);

    return err;
}


static estats_error*
_estats_conninfo_get_tcp_list(struct estats_conninfo** head, estats_agent* agent)
{
    estats_error* err = NULL; 
    estats_connection* conn;

    ErrIf(head == NULL, ESTATS_ERR_INVAL);
    *head = NULL;

    Chk(estats_agent_get_connection_head(&conn, agent));

    while (conn != NULL) { 
	estats_conninfo* conninfo = NULL; 

        Chk(_estats_conninfo_new_node(&conninfo));

	Chk(estats_connection_get_cid(&conninfo->cid, conn));
       	Chk(estats_connection_get_connection_spec(&conninfo->spec, conn));
	Chk(estats_connection_get_addrtype(&conninfo->addrtype, conn));

        Chk(_estats_conninfo_add_tail(head, conninfo));

	Chk(estats_connection_next(&conn, conn));
    }

Cleanup:
    return err;
}


static estats_error*
_estats_conninfo_get_ino_list(struct estats_conninfo** head)
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
	estats_conninfo* conninfo;
	uint32_t srcAddr;
	uint16_t srcPort;
	uint32_t dstAddr;
	uint16_t dstPort;

       	while (fgets(buf, sizeof(buf), file) != NULL) {

	    Chk(_estats_conninfo_new_node(&conninfo));

	    if ((scan = sscanf(buf,
			    "%*u: %x:%hx %x:%hx %x %*x:%*x %*x:%*x %*x %u %*u %lu",
			    (uint32_t *) &(srcAddr),
			    (uint16_t *) &(srcPort),
			    (uint32_t *) &(dstAddr),
			    (uint16_t *) &(dstPort),
			    (int *) &(conninfo->state),
			    (uid_t *) &(conninfo->uid),
			    (ino_t *) &(conninfo->ino))) == 7) { 

		Chk(estats_value_new(&conninfo->spec.src_addr, &srcAddr, ESTATS_VALUE_TYPE_IP4ADDR));
		Chk(estats_value_new(&conninfo->spec.src_port, &srcPort, ESTATS_VALUE_TYPE_UINT16));
		Chk(estats_value_new(&conninfo->spec.dst_addr, &dstAddr, ESTATS_VALUE_TYPE_IP4ADDR));
		Chk(estats_value_new(&conninfo->spec.dst_port, &dstPort, ESTATS_VALUE_TYPE_UINT16));

		conninfo->addrtype = ESTATS_ADDRTYPE_IPV4;

                Chk(_estats_conninfo_add_tail(head, conninfo));
	    } else {
	       	free(conninfo);
	    }
       	}
       	fclose(file);
    }

    if (file6) { 
	estats_conninfo* conninfo;
	char srcAddr[INET6_ADDRSTRLEN];
	uint16_t srcPort;
	char dstAddr[INET6_ADDRSTRLEN];
	uint16_t dstPort;

	while (fgets(buf, sizeof(buf), file6) != NULL) {

	    Chk(_estats_conninfo_new_node(&conninfo));

	    if ((scan = sscanf(buf,
			    "%*u: %64[0-9A-Fa-f]:%hx %64[0-9A-Fa-f]:%hx %x %*x:%*x %*x:%*x %*x %u %*u %u",
			    (char *) &srcAddr,
			    (uint16_t *) &srcPort,
			    (char *) &dstAddr,
			    (uint16_t *) &dstPort,
			    (int *) &(conninfo->state),
			    (uid_t *) &(conninfo->uid),
			    (pid_t *) &(conninfo->ino))) == 7) { 

		sscanf(srcAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]); 

		estats_value_new(&conninfo->spec.src_addr, &in6.s6_addr, ESTATS_VALUE_TYPE_IP6ADDR);
		estats_value_new(&conninfo->spec.src_port, &srcPort, ESTATS_VALUE_TYPE_UINT16);

		sscanf(dstAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]);

		estats_value_new(&conninfo->spec.dst_addr, &in6.s6_addr, ESTATS_VALUE_TYPE_IP6ADDR);
		estats_value_new(&conninfo->spec.dst_port, &dstPort, ESTATS_VALUE_TYPE_UINT16);

		conninfo->addrtype = ESTATS_ADDRTYPE_IPV6;

                Chk(_estats_conninfo_add_tail(head, conninfo));
	    } else { 
		free(conninfo);
	    }
       	}
       	fclose(file6);
    } 

Cleanup:
    return err;
}


static estats_error*
_estats_conninfo_get_pid_list(struct estats_conninfo** head)
{ 
    estats_error* err;
    estats_conninfo* ecl;
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
	    if (fddir = opendir(path))  //else lacks permissions 
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
       	
	                Chk(_estats_conninfo_new_node(&ecl));
//                        Chk(Malloc((void**) &ecl->cmdline, PATH_MAX));

			if (sscanf(buf, "Name: %2s\n", &ecl->cmdline) != 1) {
//			    Free((void**) &ecl->cmdline);
			    Free((void**) &ecl);
			    goto FileCleanup;
		       	}

			ecl->ino = st.st_ino;
		       	ecl->pid = pid;
                        Chk(_estats_conninfo_add_tail(head, ecl));

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
_estats_conninfo_add_tail(struct estats_conninfo** head, struct estats_conninfo* ci)
{
    estats_error* err = NULL;

    ErrIf(ci->next != NULL, ESTATS_ERR_INVAL);

    ci->next = *head;
    *head = ci;

Cleanup:
    return err;
}

static estats_error*
_estats_conninfo_spec_compare(int* result, const estats_conninfo* st1, const estats_conninfo* st2)
{
    estats_error* err = NULL;
    const struct estats_connection_spec* sp1;
    const struct estats_connection_spec* sp2;
    int sa, sp, da, dp;

    ErrIf(result == NULL || st1 == NULL || st2 == NULL, ESTATS_ERR_INVAL);
    ErrIf((sp1 = &st1->spec) == NULL || (sp2 = &st2->spec) == NULL, ESTATS_ERR_INVAL);

    if (st1->addrtype != st2->addrtype) {
	*result = 1;
	goto Cleanup;
    }

    Chk(estats_value_compare(&sa, sp1->src_addr, sp2->src_addr));
    Chk(estats_value_compare(&sp, sp1->src_port, sp2->src_port));
    Chk(estats_value_compare(&da, sp1->dst_addr, sp2->dst_addr));
    Chk(estats_value_compare(&dp, sp1->dst_port, sp2->dst_port));

    if (!sa && !sp && !da && !dp) *result = 0;
    else *result = 1;

Cleanup:
    return err;
}
