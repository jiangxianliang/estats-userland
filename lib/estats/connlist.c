#include <estats/estats-int.h>


static estats_error* _estats_connlist_refresh(estats_connlist** _connlist, estats_agent* agent);
static estats_error* _estats_connlist_get_tcp_list(struct estats_connlist** tcp_list, estats_agent* agent);
static estats_error* _estats_connlist_get_ino_list(struct estats_connlist** ino_list);
static estats_error* _estats_connlist_get_pid_list(struct estats_connlist** pid_list);
static estats_error* _estats_connlist_spec_compare(int* res, const estats_connlist* st1, const estats_connlist* st2);
static void          _estats_connlist_free_node(estats_connlist** connlist);
static void          _estats_connlist_remove_node(estats_connlist* connlist);

estats_error*
estats_connlist_head(estats_connlist** connlist, estats_agent* agent)
{
    estats_error* err = NULL;
    struct estats_list* head;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_connlist_refresh(connlist, agent));

    head = &((*connlist)->list);
    *connlist = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_connlist, list);

Cleanup:
    return err;
}

estats_error*
estats_connlist_next(estats_connlist** next, const estats_connlist* prev)
{
    estats_error* err = NULL;
    struct estats_list* l;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    l = prev->list.next;
    if (ESTATS_LIST_ENTRY(l, estats_connlist, list)->pos == CONNLIST_IS_HEAD)
        *next = NULL;
    else
        *next = ESTATS_LIST_ENTRY(l, estats_connlist, list);

Cleanup:
    return err;
}

estats_error*
estats_connlist_head_matched(estats_connlist** connlist, estats_agent* agent,
			ConnlistMask mask,
			const int cid,
			const int pid,
			const int uid,
			const char *cmdline,
			const estats_value* srcport,
		       	const estats_value* dstaddr,
		       	const estats_value* dstport)
{
    estats_error* err = NULL;
    struct estats_list* head;
    struct estats_list* currItem;
    struct estats_list* tmp;
    int cmp;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_connlist_refresh(connlist, agent));

    head = &((*connlist)->list);
    ESTATS_LIST_FOREACH_SAFE(currItem, tmp, head) {
	estats_connlist* currStat = ESTATS_LIST_ENTRY(currItem, estats_connlist, list);

	if (mask & CONNLIST_CID) {
	    if (cid != currStat->cid) _estats_connlist_remove_node(currStat);
	}
	if (mask & CONNLIST_PID) {
	    if (pid != currStat->pid) _estats_connlist_remove_node(currStat);
	}
	if (mask & CONNLIST_UID) {
	    if (uid != currStat->uid) _estats_connlist_remove_node(currStat);
	}
	if (mask & CONNLIST_CMD) {
	    if (fnmatch(cmdline, currStat->cmdline, FNM_CASEFOLD))
		_estats_connlist_remove_node(currStat);
	}
	if (mask & CONNLIST_SRCPORT) {
	    estats_value_compare(&cmp, srcport, currStat->spec.src_port);
	    if (cmp) _estats_connlist_remove_node(currStat);
	}
	if (mask & CONNLIST_DSTADDR) {
	    estats_value_compare(&cmp, dstaddr, currStat->spec.dst_addr);
	    if (cmp) _estats_connlist_remove_node(currStat);
	}
	if (mask & CONNLIST_DSTPORT) {
	    estats_value_compare(&cmp, dstport, currStat->spec.dst_port);
	    if (cmp) _estats_connlist_remove_node(currStat);
       	}
    }

    *connlist = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_connlist, list);

Cleanup:
    return err;
}

void
estats_connlist_free(estats_connlist** connlist)
{
  /** Make sure connlist is the head **/
//  while (connlist->pos != CONNLIST_IS_HEAD) {
//  } 

  if (connlist == NULL || *connlist == NULL)
    return;

  
}

void
_estats_connlist_free_node(estats_connlist** connlist)
{
    if (connlist == NULL || *connlist == NULL)
	return;

    estats_value_free(&((*connlist)->spec.src_addr));
    estats_value_free(&((*connlist)->spec.src_port));
    estats_value_free(&((*connlist)->spec.dst_addr));
    estats_value_free(&((*connlist)->spec.dst_port));

    Free((void**) connlist);
}

void
_estats_connlist_remove_node(estats_connlist* connlist)
{ 
    _estats_list_del(&connlist->list);
    _estats_connlist_free_node(&connlist);
}

estats_error*
estats_connlist_foreach(estats_connlist* ecl,
                    estats_connlist_foreach_func f,
                    void* userData)
{
    estats_error* err = NULL;
    struct estats_list* head;
    struct estats_list* currItem;

    ErrIf(ecl == NULL || f == NULL, ESTATS_ERR_INVAL);

    head = &(ecl->list);
    ESTATS_LIST_FOREACH(currItem, head) {
        int flags = 0;
        estats_connlist* currStat = ESTATS_LIST_ENTRY(currItem, estats_connlist, list);

        if (currItem->prev == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_FIRST;
        if (currItem->next == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_LAST;
        
        if (f(currStat, flags, userData) == ESTATS_FOREACH_STOP)
            break;
    }
Cleanup:
    return err; 
}


estats_error*
estats_connlist_get_cid(int* cid, const estats_connlist* ecl)
{
    estats_error* err = NULL;

    ErrIf(cid == NULL || ecl == NULL, ESTATS_ERR_INVAL);
    *cid = ecl->cid;

Cleanup:
    return err;
}


estats_error*
estats_connlist_get_pid(int* pid, const estats_connlist* connlist)
{
    estats_error* err = NULL;

    ErrIf(pid == NULL || connlist == NULL, ESTATS_ERR_INVAL);
    *pid = connlist->pid;

Cleanup:
    return err;
}


estats_error*
estats_connlist_get_uid(int* uid, const estats_connlist* connlist)
{
    estats_error* err = NULL;

    ErrIf(uid == NULL || connlist == NULL, ESTATS_ERR_INVAL);
    *uid = connlist->uid;

Cleanup:
    return err;
}


estats_error*
estats_connlist_get_state(int* state, const estats_connlist* connlist)
{
    estats_error* err = NULL;

    ErrIf(state == NULL || connlist == NULL, ESTATS_ERR_INVAL);
    *state = connlist->state;

Cleanup:
    return err;
}


estats_error*
estats_connlist_get_spec(struct estats_connection_spec *spec, const estats_connlist* st)
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
estats_connlist_get_cmdline(char** str, const estats_connlist* connlist)
{
    estats_error* err = NULL;

    ErrIf(connlist == NULL, ESTATS_ERR_INVAL);
    Chk(Strdup(str, connlist->cmdline));

Cleanup:
    return err;
}


static estats_error*
_estats_connlist_refresh(struct estats_connlist** ecl, estats_agent* agent)
{
    estats_error* err = NULL;
    estats_connlist* tcp;
    estats_connlist* ino;
    estats_connlist* pid;
    estats_connlist* newcl;
    struct estats_list *tcpPos, *tcpHead;
    struct estats_list *inoPos, *inoHead;
    struct estats_list *pidPos, *pidHead; 
    int dif;
    int tcp_entry, fd_entry;

    /* associate cid with address */ 
    Chk(_estats_connlist_get_tcp_list(&tcp, agent));

    /* associate address with ino */
    Chk(_estats_connlist_get_ino_list(&ino));

    /* associate ino with pid */
    Chk(_estats_connlist_get_pid_list(&pid));

    /* collate above */

    Chk(Malloc((void**) ecl, sizeof(estats_connlist)));
    _estats_list_init(&((*ecl)->list));
    (*ecl)->pos = CONNLIST_IS_HEAD;

    tcpHead = &tcp->list;
    ESTATS_LIST_FOREACH(tcpPos, tcpHead) {

	estats_connlist* tconnlist = ESTATS_LIST_ENTRY(tcpPos, estats_connlist, list);

	tcp_entry = 0;

	inoHead = &ino->list;
       	ESTATS_LIST_FOREACH(inoPos, inoHead) {

	    estats_connlist* icl = ESTATS_LIST_ENTRY(inoPos, estats_connlist, list);

	    Chk(_estats_connlist_spec_compare(&dif, icl, tconnlist));

	    if (!dif) {
	       	tcp_entry = 1;
	       	fd_entry = 0;

		pidHead = &pid->list;
	       	ESTATS_LIST_FOREACH(pidPos, pidHead) {

		    estats_connlist* pcl = ESTATS_LIST_ENTRY(pidPos, estats_connlist, list);

		    if(pcl->ino == icl->ino) { //then create entry 
			fd_entry = 1;

			Chk(Malloc((void**) &newcl, sizeof(estats_connlist)));

			newcl->pid = pcl->pid; 
			Chk(Strdup(&newcl->cmdline, pcl->cmdline));
//			strncpy(newcl->cmdline, pcl->cmdline, PATH_MAX); 
//			strncpy(newcl->cmdline, pcl->cmdline, 256); 

newcl->uid = icl->uid;
		       	newcl->state = icl->state;

			newcl->cid = tconnlist->cid;

			newcl->addrtype = tconnlist->addrtype; 

			Chk(estats_connlist_get_spec(&newcl->spec, tconnlist));

			_estats_list_add_tail(&newcl->list, &((*ecl)->list));
			newcl->pos = CONNLIST_IS_NOT_HEAD;
		    }
	       	}
	       	if(!fd_entry) { // add entry w/out cmdline 
			Chk(Malloc((void**) &newcl, sizeof(estats_connlist)));

			newcl->pid = 0;
			newcl->uid = icl->uid;
		       	newcl->state = icl->state;

			newcl->cid = tconnlist->cid;
			newcl->addrtype = tconnlist->addrtype; 
			Chk(estats_connlist_get_spec(&newcl->spec, tconnlist));

			Chk(Strdup(&newcl->cmdline, "\0")); 

			_estats_list_add_tail(&newcl->list, &((*ecl)->list));
			newcl->pos = CONNLIST_IS_NOT_HEAD;
	       	}
	    } 
	} 
	if(!tcp_entry) { // then connection has vanished; add residual cid info
			 // (only for consistency with entries in /proc/web100) 
	    Chk(Malloc((void**) &newcl, sizeof(estats_connlist))); 

	    newcl->cid = tconnlist->cid; 
	    newcl->addrtype = tconnlist->addrtype; 
	    Chk(estats_connlist_get_spec(&newcl->spec, tconnlist));
	    Chk(Strdup(&newcl->cmdline, "\0")); 

	    _estats_list_add_tail(&newcl->list, &((*ecl)->list));
	    newcl->pos = CONNLIST_IS_NOT_HEAD;
       	} 
    }
Cleanup:
    return err;
}


static estats_error*
_estats_connlist_get_tcp_list(struct estats_connlist** head, estats_agent* agent)
{
    estats_error* err = NULL; 
    estats_connection* conn;
    struct estats_list* connHead;
    struct estats_list* connPos;

    Chk(Malloc((void**) head, sizeof(estats_connlist)));
    _estats_list_init(&((*head)->list));

    Chk(estats_agent_get_connection_head(&conn, agent));
/*    
    if (conn != NULL) {
       	connHead = &(conn->list);

	ESTATS_LIST_FOREACH(connPos, connHead) {
	    estats_connlist* connlist;
	    estats_connection* cp = ESTATS_LIST_ENTRY(connPos, estats_connection, list);

	    Chk(Malloc((void**) &connlist, sizeof(estats_connlist)));
	    _estats_list_init(&(connlist->list));

	    Chk(estats_connection_get_cid(&connlist->cid, cp));
	    Chk(estats_connection_get_spec(&connlist->spec, cp));
	    printf("tcplist\n");
	    _estats_list_add_tail(&connlist->list, &((*head)->list));
       	}
    }
*/

    while (conn != NULL) { 
	estats_connlist* connlist; 

	Chk(Malloc((void**) &connlist, sizeof(estats_connlist)));

	Chk(estats_connection_get_cid(&connlist->cid, conn));
       	Chk(estats_connection_get_spec(&connlist->spec, conn));
	Chk(estats_connection_get_addrtype(&connlist->addrtype, conn));

       	_estats_list_add_tail(&connlist->list, &((*head)->list));
       
	Chk(estats_connection_next(&conn, conn));
    }

Cleanup:
    return err;
}


static estats_error*
_estats_connlist_get_ino_list(struct estats_connlist** head)
{ 
    estats_error* err = NULL;
    FILE *file;
    FILE *file6;
    char buf[256];
    int scan;
    struct in6_addr in6;

    Chk(Malloc((void**) head, sizeof(estats_connlist)));
    _estats_list_init(&((*head)->list));

    file = fopen("/proc/net/tcp", "r");
    file6 = fopen("/proc/net/tcp6", "r");

    if (file) {
	estats_connlist* connlist;
	uint32_t srcAddr;
	uint16_t srcPort;
	uint32_t dstAddr;
	uint16_t dstPort;

       	while (fgets(buf, sizeof(buf), file) != NULL) { 

	    Chk(Malloc((void**) &connlist, sizeof(estats_connlist)));

	    if ((scan = sscanf(buf,
			    "%*u: %x:%hx %x:%hx %x %*x:%*x %*x:%*x %*x %u %*u %lu",
			    (uint32_t *) &(srcAddr),
			    (uint16_t *) &(srcPort),
			    (uint32_t *) &(dstAddr),
			    (uint16_t *) &(dstPort),
			    (int *) &(connlist->state),
			    (uid_t *) &(connlist->uid),
			    (ino_t *) &(connlist->ino))) == 7) { 

		estats_value_new(&connlist->spec.src_addr, &srcAddr, ESTATS_VALUE_TYPE_IP4ADDR);
		estats_value_new(&connlist->spec.src_port, &srcPort, ESTATS_VALUE_TYPE_UINT16);
		estats_value_new(&connlist->spec.dst_addr, &dstAddr, ESTATS_VALUE_TYPE_IP4ADDR);
		estats_value_new(&connlist->spec.dst_port, &dstPort, ESTATS_VALUE_TYPE_UINT16);

		connlist->addrtype = ESTATS_ADDRTYPE_IPV4;

		_estats_list_add_tail(&connlist->list, &((*head)->list));
	    } else {
	       	free(connlist);
	    }
       	}
       	fclose(file);
    }

    if (file6) { 
	estats_connlist* connlist;
	char srcAddr[INET6_ADDRSTRLEN];
	uint16_t srcPort;
	char dstAddr[INET6_ADDRSTRLEN];
	uint16_t dstPort;

	while (fgets(buf, sizeof(buf), file6) != NULL) { 

	    Chk(Malloc((void**) &connlist, sizeof(estats_connlist)));

	    if ((scan = sscanf(buf,
			    "%*u: %64[0-9A-Fa-f]:%hx %64[0-9A-Fa-f]:%hx %x %*x:%*x %*x:%*x %*x %u %*u %u",
			    (char *) &srcAddr,
			    (uint16_t *) &srcPort,
			    (char *) &dstAddr,
			    (uint16_t *) &dstPort,
			    (int *) &(connlist->state),
			    (uid_t *) &(connlist->uid),
			    (pid_t *) &(connlist->ino))) == 7) { 

		sscanf(srcAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]); 

		estats_value_new(&connlist->spec.src_addr, &in6.s6_addr, ESTATS_VALUE_TYPE_IP6ADDR);
		estats_value_new(&connlist->spec.src_port, &srcPort, ESTATS_VALUE_TYPE_UINT16);

		sscanf(dstAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]);

		estats_value_new(&connlist->spec.dst_addr, &in6.s6_addr, ESTATS_VALUE_TYPE_IP6ADDR);
		estats_value_new(&connlist->spec.dst_port, &dstPort, ESTATS_VALUE_TYPE_UINT16);

		connlist->addrtype = ESTATS_ADDRTYPE_IPV6;

	       	_estats_list_add_tail(&connlist->list, &((*head)->list));
	    } else { 
		free(connlist);
	    }
       	}
       	fclose(file6);
    } 

Cleanup:
    return err;
}


static estats_error*
_estats_connlist_get_pid_list(struct estats_connlist** head)
{ 
    estats_error* err;
    estats_connlist* ecl;
    DIR *dir, *fddir; 
    struct dirent *direntp, *fddirentp;
    pid_t pid;
    char path[PATH_MAX];
    char buf[256];
    struct stat st;
    int stno;
    FILE* file;
    
    Chk(Malloc((void**) head, sizeof(estats_connlist)));
    _estats_list_init(&((*head)->list));

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
		       	
			Chk(Malloc((void**) &ecl, sizeof(estats_connlist))); 
			Chk(Malloc((void**) &ecl->cmdline, PATH_MAX));

			if (sscanf(buf, "Name: %s\n", ecl->cmdline) != 1) {
			    Free((void**) &ecl->cmdline);
			    Free((void**) &ecl);
			    goto FileCleanup;
		       	}

			ecl->ino = st.st_ino;
		       	ecl->pid = pid;

			_estats_list_add_tail(&ecl->list, &((*head)->list));
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
_estats_connlist_spec_compare(int* result, const estats_connlist* st1, const estats_connlist* st2)
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
