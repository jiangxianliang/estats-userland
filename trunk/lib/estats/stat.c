#include <estats/estats-int.h>


static estats_error* _estats_stat_refresh(estats_stat** _stat, estats_agent* agent);
static estats_error* _estats_stat_get_tcp_list(struct estats_stat** tcp_list, estats_agent* agent);
static estats_error* _estats_stat_get_ino_list(struct estats_stat** ino_list);
static estats_error* _estats_stat_get_pid_list(struct estats_stat** pid_list);
static estats_error* _estats_stat_spec_compare(int* res, const estats_stat* st1, const estats_stat* st2);
static void          _estats_stat_free_node(estats_stat** stat);
static void          _estats_stat_remove_node(estats_stat* stat);

estats_error*
estats_stat_head(estats_stat** stat, estats_agent* agent)
{
    estats_error* err = NULL;
    struct estats_list* head;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_stat_refresh(stat, agent));

    head = &((*stat)->list);
    *stat = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_stat, list);

Cleanup:
    return err;
}

estats_error*
estats_stat_next(estats_stat** next, const estats_stat* prev)
{
    estats_error* err = NULL;
    struct estats_list* l;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    l = prev->list.next;
    if (ESTATS_LIST_ENTRY(l, estats_stat, list)->pos == STAT_IS_HEAD)
        *next = NULL;
    else
        *next = ESTATS_LIST_ENTRY(l, estats_stat, list);

Cleanup:
    return err;
}

estats_error*
estats_stat_head_matched(estats_stat** stat, estats_agent* agent,
			StatMask mask,
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

    Chk(_estats_stat_refresh(stat, agent));

    head = &((*stat)->list);
    ESTATS_LIST_FOREACH_SAFE(currItem, tmp, head) {
	estats_stat* currStat = ESTATS_LIST_ENTRY(currItem, estats_stat, list);

	if (mask & STAT_CID) {
	    if (cid != currStat->cid) _estats_stat_remove_node(currStat);
	}
	if (mask & STAT_PID) {
	    if (pid != currStat->pid) _estats_stat_remove_node(currStat);
	}
	if (mask & STAT_UID) {
	    if (uid != currStat->uid) _estats_stat_remove_node(currStat);
	}
	if (mask & STAT_CMD) {
	    if (fnmatch(cmdline, currStat->cmdline, FNM_CASEFOLD))
		_estats_stat_remove_node(currStat);
	}
	if (mask & STAT_SRCPORT) {
	    estats_value_compare(&cmp, srcport, currStat->spec.src_port);
	    if (cmp) _estats_stat_remove_node(currStat);
	}
	if (mask & STAT_DSTADDR) {
	    estats_value_compare(&cmp, dstaddr, currStat->spec.dst_addr);
	    if (cmp) _estats_stat_remove_node(currStat);
	}
	if (mask & STAT_DSTPORT) {
	    estats_value_compare(&cmp, dstport, currStat->spec.dst_port);
	    if (cmp) _estats_stat_remove_node(currStat);
       	}
    }

    *stat = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_stat, list);

Cleanup:
    return err;
}

void
estats_stat_free(estats_stat** stat)
{
  /** Make sure stat is the head **/
//  while (stat->pos != STAT_IS_HEAD) {
//  } 

  if (stat == NULL || *stat == NULL)
    return;

  
}

void
_estats_stat_free_node(estats_stat** stat)
{
    if (stat == NULL || *stat == NULL)
	return;

    estats_value_free(&((*stat)->spec.src_addr));
    estats_value_free(&((*stat)->spec.src_port));
    estats_value_free(&((*stat)->spec.dst_addr));
    estats_value_free(&((*stat)->spec.dst_port));

    Free((void**) stat);
}

void
_estats_stat_remove_node(estats_stat* stat)
{ 
    _estats_list_del(&stat->list);
    _estats_stat_free_node(&stat);
}

estats_error*
estats_stat_foreach(estats_stat* wstat,
                    estats_stat_foreach_func f,
                    void* userData)
{
    estats_error* err = NULL;
    struct estats_list* head;
    struct estats_list* currItem;

    ErrIf(wstat == NULL || f == NULL, ESTATS_ERR_INVAL);

    head = &(wstat->list);
    ESTATS_LIST_FOREACH(currItem, head) {
        int flags = 0;
        estats_stat* currStat = ESTATS_LIST_ENTRY(currItem, estats_stat, list);

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
estats_stat_get_cid(int* cid, const estats_stat* wstat)
{
    estats_error* err = NULL;

    ErrIf(cid == NULL || wstat == NULL, ESTATS_ERR_INVAL);
    *cid = wstat->cid;

Cleanup:
    return err;
}


estats_error*
estats_stat_get_pid(int* pid, const estats_stat* stat)
{
    estats_error* err = NULL;

    ErrIf(pid == NULL || stat == NULL, ESTATS_ERR_INVAL);
    *pid = stat->pid;

Cleanup:
    return err;
}


estats_error*
estats_stat_get_uid(int* uid, const estats_stat* stat)
{
    estats_error* err = NULL;

    ErrIf(uid == NULL || stat == NULL, ESTATS_ERR_INVAL);
    *uid = stat->uid;

Cleanup:
    return err;
}


estats_error*
estats_stat_get_state(int* state, const estats_stat* stat)
{
    estats_error* err = NULL;

    ErrIf(state == NULL || stat == NULL, ESTATS_ERR_INVAL);
    *state = stat->state;

Cleanup:
    return err;
}


estats_error*
estats_stat_get_spec(struct estats_connection_spec *spec, const estats_stat* st)
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
estats_stat_get_cmdline(char** str, const estats_stat* stat)
{
    estats_error* err = NULL;

    ErrIf(stat == NULL, ESTATS_ERR_INVAL);
    Chk(Strdup(str, stat->cmdline));

Cleanup:
    return err;
}


static estats_error*
_estats_stat_refresh(struct estats_stat** wstat, estats_agent* agent)
{
    estats_error* err = NULL;
    estats_stat* tcp;
    estats_stat* ino;
    estats_stat* pid;
    estats_stat* newstat;
    struct estats_list *tcpPos, *tcpHead;
    struct estats_list *inoPos, *inoHead;
    struct estats_list *pidPos, *pidHead; 
    int dif;
    int tcp_entry, fd_entry;

    /* associate cid with address */ 
    Chk(_estats_stat_get_tcp_list(&tcp, agent));

    /* associate address with ino */
    Chk(_estats_stat_get_ino_list(&ino));

    /* associate ino with pid */
    Chk(_estats_stat_get_pid_list(&pid));

    /* collate above */

    Chk(Malloc((void**) wstat, sizeof(estats_stat)));
    _estats_list_init(&((*wstat)->list));
    (*wstat)->pos = STAT_IS_HEAD;

    tcpHead = &tcp->list;
    ESTATS_LIST_FOREACH(tcpPos, tcpHead) {

	estats_stat* tstat = ESTATS_LIST_ENTRY(tcpPos, estats_stat, list);

	tcp_entry = 0;

	inoHead = &ino->list;
       	ESTATS_LIST_FOREACH(inoPos, inoHead) {

	    estats_stat* istat = ESTATS_LIST_ENTRY(inoPos, estats_stat, list);

	    Chk(_estats_stat_spec_compare(&dif, istat, tstat));

	    if (!dif) {
	       	tcp_entry = 1;
	       	fd_entry = 0;

		pidHead = &pid->list;
	       	ESTATS_LIST_FOREACH(pidPos, pidHead) {

		    estats_stat* pstat = ESTATS_LIST_ENTRY(pidPos, estats_stat, list);

		    if(pstat->ino == istat->ino) { //then create entry 
			fd_entry = 1;

			Chk(Malloc((void**) &newstat, sizeof(estats_stat)));

			newstat->pid = pstat->pid; 
			Chk(Strdup(&newstat->cmdline, pstat->cmdline));
//			strncpy(newstat->cmdline, pstat->cmdline, PATH_MAX); 
//			strncpy(newstat->cmdline, pstat->cmdline, 256); 

newstat->uid = istat->uid;
		       	newstat->state = istat->state;

			newstat->cid = tstat->cid;

			newstat->addrtype = tstat->addrtype; 

			Chk(estats_stat_get_spec(&newstat->spec, tstat));

			_estats_list_add_tail(&newstat->list, &((*wstat)->list));
			newstat->pos = STAT_IS_NOT_HEAD;
		    }
	       	}
	       	if(!fd_entry) { // add entry w/out cmdline 
			Chk(Malloc((void**) &newstat, sizeof(estats_stat)));

			newstat->pid = 0;
			newstat->uid = istat->uid;
		       	newstat->state = istat->state;

			newstat->cid = tstat->cid;
			newstat->addrtype = tstat->addrtype; 
			Chk(estats_stat_get_spec(&newstat->spec, tstat));

			Chk(Strdup(&newstat->cmdline, "\0")); 

			_estats_list_add_tail(&newstat->list, &((*wstat)->list));
			newstat->pos = STAT_IS_NOT_HEAD;
	       	}
	    } 
	} 
	if(!tcp_entry) { // then connection has vanished; add residual cid info
			 // (only for consistency with entries in /proc/web100) 
	    Chk(Malloc((void**) &newstat, sizeof(estats_stat))); 

	    newstat->cid = tstat->cid; 
	    newstat->addrtype = tstat->addrtype; 
	    Chk(estats_stat_get_spec(&newstat->spec, tstat));
	    Chk(Strdup(&newstat->cmdline, "\0")); 

	    _estats_list_add_tail(&newstat->list, &((*wstat)->list));
	    newstat->pos = STAT_IS_NOT_HEAD;
       	} 
    }
Cleanup:
    return err;
}


static estats_error*
_estats_stat_get_tcp_list(struct estats_stat** head, estats_agent* agent)
{
    estats_error* err = NULL; 
    estats_connection* conn;
    struct estats_list* connHead;
    struct estats_list* connPos;

    Chk(Malloc((void**) head, sizeof(estats_stat)));
    _estats_list_init(&((*head)->list));

    Chk(estats_agent_get_connection_head(&conn, agent));
/*    
    if (conn != NULL) {
       	connHead = &(conn->list);

	ESTATS_LIST_FOREACH(connPos, connHead) {
	    estats_stat* stat;
	    estats_connection* cp = ESTATS_LIST_ENTRY(connPos, estats_connection, list);

	    Chk(Malloc((void**) &stat, sizeof(estats_stat)));
	    _estats_list_init(&(stat->list));

	    Chk(estats_connection_get_cid(&stat->cid, cp));
	    Chk(estats_connection_get_spec(&stat->spec, cp));
	    printf("tcplist\n");
	    _estats_list_add_tail(&stat->list, &((*head)->list));
       	}
    }
*/

    while (conn != NULL) { 
	estats_stat* stat; 

	Chk(Malloc((void**) &stat, sizeof(estats_stat)));

	Chk(estats_connection_get_cid(&stat->cid, conn));
       	Chk(estats_connection_get_spec(&stat->spec, conn));
	Chk(estats_connection_get_addrtype(&stat->addrtype, conn));

       	_estats_list_add_tail(&stat->list, &((*head)->list));
       
	Chk(estats_connection_next(&conn, conn));
    }

Cleanup:
    return err;
}


static estats_error*
_estats_stat_get_ino_list(struct estats_stat** head)
{ 
    estats_error* err = NULL;
    FILE *file;
    FILE *file6;
    char buf[256];
    int scan;
    struct in6_addr in6;

    Chk(Malloc((void**) head, sizeof(estats_stat)));
    _estats_list_init(&((*head)->list));

    file = fopen("/proc/net/tcp", "r");
    file6 = fopen("/proc/net/tcp6", "r");

    if (file) {
	estats_stat* stat;
	uint32_t srcAddr;
	uint16_t srcPort;
	uint32_t dstAddr;
	uint16_t dstPort;

       	while (fgets(buf, sizeof(buf), file) != NULL) { 

	    Chk(Malloc((void**) &stat, sizeof(estats_stat)));

	    if ((scan = sscanf(buf,
			    "%*u: %x:%hx %x:%hx %x %*x:%*x %*x:%*x %*x %u %*u %lu",
			    (uint32_t *) &(srcAddr),
			    (uint16_t *) &(srcPort),
			    (uint32_t *) &(dstAddr),
			    (uint16_t *) &(dstPort),
			    (int *) &(stat->state),
			    (uid_t *) &(stat->uid),
			    (ino_t *) &(stat->ino))) == 7) { 

		estats_value_new(&stat->spec.src_addr, &srcAddr, ESTATS_VALUE_TYPE_IP4ADDR);
		estats_value_new(&stat->spec.src_port, &srcPort, ESTATS_VALUE_TYPE_UINT16);
		estats_value_new(&stat->spec.dst_addr, &dstAddr, ESTATS_VALUE_TYPE_IP4ADDR);
		estats_value_new(&stat->spec.dst_port, &dstPort, ESTATS_VALUE_TYPE_UINT16);

		stat->addrtype = ESTATS_ADDRTYPE_IPV4;

		_estats_list_add_tail(&stat->list, &((*head)->list));
	    } else {
	       	free(stat);
	    }
       	}
       	fclose(file);
    }

    if (file6) { 
	estats_stat* stat;
	char srcAddr[INET6_ADDRSTRLEN];
	uint16_t srcPort;
	char dstAddr[INET6_ADDRSTRLEN];
	uint16_t dstPort;

	while (fgets(buf, sizeof(buf), file6) != NULL) { 

	    Chk(Malloc((void**) &stat, sizeof(estats_stat)));

	    if ((scan = sscanf(buf,
			    "%*u: %64[0-9A-Fa-f]:%hx %64[0-9A-Fa-f]:%hx %x %*x:%*x %*x:%*x %*x %u %*u %u",
			    (char *) &srcAddr,
			    (uint16_t *) &srcPort,
			    (char *) &dstAddr,
			    (uint16_t *) &dstPort,
			    (int *) &(stat->state),
			    (uid_t *) &(stat->uid),
			    (pid_t *) &(stat->ino))) == 7) { 

		sscanf(srcAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]); 

		estats_value_new(&stat->spec.src_addr, &in6.s6_addr, ESTATS_VALUE_TYPE_IP6ADDR);
		estats_value_new(&stat->spec.src_port, &srcPort, ESTATS_VALUE_TYPE_UINT16);

		sscanf(dstAddr, "%8x%8x%8x%8x", &in6.s6_addr32[0], &in6.s6_addr32[1], &in6.s6_addr32[2], &in6.s6_addr32[3]);

		estats_value_new(&stat->spec.dst_addr, &in6.s6_addr, ESTATS_VALUE_TYPE_IP6ADDR);
		estats_value_new(&stat->spec.dst_port, &dstPort, ESTATS_VALUE_TYPE_UINT16);

		stat->addrtype = ESTATS_ADDRTYPE_IPV6;

	       	_estats_list_add_tail(&stat->list, &((*head)->list));
	    } else { 
		free(stat);
	    }
       	}
       	fclose(file6);
    } 

Cleanup:
    return err;
}


static estats_error*
_estats_stat_get_pid_list(struct estats_stat** head)
{ 
    estats_error* err;
    estats_stat* wstat;
    DIR *dir, *fddir; 
    struct dirent *direntp, *fddirentp;
    pid_t pid;
    char path[PATH_MAX];
    char buf[256];
    struct stat st;
    int stno;
    FILE* file;
    
    Chk(Malloc((void**) head, sizeof(estats_stat)));
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
		       	
			Chk(Malloc((void**) &wstat, sizeof(estats_stat))); 
			Chk(Malloc((void**) &wstat->cmdline, PATH_MAX));

			if (sscanf(buf, "Name: %s\n", wstat->cmdline) != 1) {
			    Free((void**) &wstat->cmdline);
			    Free((void**) &wstat);
			    goto FileCleanup;
		       	}

			wstat->ino = st.st_ino;
		       	wstat->pid = pid;

			_estats_list_add_tail(&wstat->list, &((*head)->list));
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
_estats_stat_spec_compare(int* result, const estats_stat* st1, const estats_stat* st2)
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
