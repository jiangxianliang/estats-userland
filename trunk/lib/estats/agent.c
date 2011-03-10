#include <estats/estats-int.h>

static estats_error* _estats_agent_refresh_connections(estats_agent *agent);
static estats_error* _estats_agent_attach_local(estats_agent** agent);
static estats_error* _estats_agent_attach_log(estats_agent** agent, FILE *header);
static void          _estats_agent_detach_local(estats_agent** agent);
static void          _estats_agent_detach_log(estats_agent** agent);


estats_error*
_estats_agent_parse_header(estats_agent* agent, FILE* fp)
{
    estats_error* err = NULL;
    estats_group* group = NULL;
    estats_group* curGroup = NULL; /* DO NOT FREE */
    estats_var* var = NULL;
    char linebuf[256];
    int have_len = 0;

    ErrIf(agent == NULL || fp == NULL, ESTATS_ERR_INVAL);

    /* First line is version string */
    Chk(Fgets(linebuf, sizeof(linebuf), fp));
    strlcpy(agent->version, linebuf, sizeof(agent->version));

    if (strncmp(agent->version, "1.", 2) != 0)
        have_len = 1;

    while (1) {
        size_t len;
        
        if ((err = Fgets(linebuf, sizeof(linebuf), fp)) != NULL) {
            if (estats_error_get_number(err) == ESTATS_ERR_EOF) {
                dbgprintf("   ... caught expected EOF at %s:%d in function %s\n", __FILE__, __LINE__, __FUNCTION__);
                estats_error_free(&err);
                break;
            } else {
                goto Cleanup;
            }
        }

        /* Trim out trailing \n if it exists */
        len = strlen(linebuf);
        if (len > 0 && linebuf[len - 1] == '\n')
            linebuf[len - 1] = '\0';

        /* Skip over blank lines */
        if (strisspace(linebuf))
            continue;

        if (linebuf[0] == '/') {
            /* Add a new group */
            Free((void**) &group);
            Chk(Malloc((void**) &group, sizeof(estats_group)));
            group->size = 0;
            group->nvars = 0;
            group->agent = agent;
            _estats_list_init(&(group->var_head.list));

            strlcpy(group->name, linebuf + 1, sizeof(group->name));

            dbgprintf("New group: %s\n", group->name);

            /* Distinguish between the group we are working on and the
               'current' group to use in the var logic.  If we don't do
               this right we may have a double-free scenario. */
            curGroup = group;

            if (strcmp(group->name, "spec") == 0) {
                agent->spec = group;
                group = NULL;
            } else {
                _estats_list_add_tail(&(group->list), &(agent->group_head.list));
                group = NULL;
            }
        } else {
            int nRead;
            int fsize;
            int varsize;
            
            /* Add a new variable */
            ErrIf(curGroup == NULL, ESTATS_ERR_HEADER);

            Chk(Malloc((void**) &var, sizeof(estats_var)));
            var->group = curGroup;

            if (!have_len) {
                Chk(Sscanf(&nRead, linebuf, "%s%d%d", var->name, &var->offset, &var->type));
                ErrIf(nRead != 3, ESTATS_ERR_HEADER);
                var->len = -1;
            } else {
                Chk(Sscanf(&nRead, linebuf, "%s%d%d%d", var->name, &var->offset, &var->type, &var->len));
                ErrIf(nRead != 4, ESTATS_ERR_HEADER);
            }

            /* Deprecated variable check */
            var->flags = 0;
            if (var->name[0] == '_') {
                var->flags |= ESTATS_VAR_FL_DEP;
                memmove(var->name, var->name + 1, strlen(var->name)); /* Purposely move the NULL byte */
            }

#if defined(DEBUG)
            dbgprintf("    New ");
            if (var->flags & ESTATS_VAR_FL_DEP)
                dbgprintf_no_prefix("(deprecated) ");
            dbgprintf_no_prefix("var: %s offset=%d type=%d len=%d\n", var->name, var->offset, var->type, var->len);
#endif /* defined(DEBUG) */

            if ((err = _estats_var_size_from_type(&varsize, var->type)) != NULL) {
                estats_error_free(&err);
                varsize = 0;
            }
            
            /* increment group (== file) size if necessary */ 
            fsize = var->offset + varsize;
            curGroup->size = ((curGroup->size < fsize) ? fsize : curGroup->size); 

            /* if size_from_type 0 (i.e., type unrecognized),
               forgo adding the variable */
            if (varsize == 0) {
                Free((void**) &var);
                continue;
            }

            curGroup->nvars++;
            _estats_list_add_tail(&(var->list), &(curGroup->var_head.list));
            var = NULL;
        }
    }

Cleanup:
    if (err != NULL) {
       	Free((void**) &group);
       	Free((void**) &var);
    }

    return err;
}


estats_error*
estats_agent_attach(estats_agent** agent, const ESTATS_AGENT_TYPE type, const void *data)
{
    estats_error* err = NULL;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);
    Chk(Malloc((void**) agent, sizeof(estats_agent)));
    _estats_list_init(&((*agent)->group_head.list));
    _estats_list_init(&((*agent)->connection_head.list));

    switch (type) {
    case ESTATS_AGENT_TYPE_LOCAL:
       	Chk(_estats_agent_attach_local(agent));
       	break;
    case ESTATS_AGENT_TYPE_LOG:
	Chk(_estats_agent_attach_log(agent, (FILE*) data));
	break;
    default:
	Err(ESTATS_ERR_AGENT_TYPE);
        break; /* not reached */
    }

Cleanup:
    if (err != NULL) {
	Free((void**) agent);
    }

    return err;
}


void
estats_agent_detach(estats_agent** agent)
{
    struct estats_list* groupCurrPos;
    struct estats_list* varCurrPos;
    struct estats_list* connCurrPos;
    struct estats_list* tmp;

    if (agent == NULL || *agent == NULL)
        return;

    ESTATS_LIST_FOREACH_SAFE(groupCurrPos, tmp, &((*agent)->group_head.list)) {
        struct estats_list* tmp2;
        estats_group* currGroup = ESTATS_LIST_ENTRY(groupCurrPos, estats_group, list);
        
        ESTATS_LIST_FOREACH_SAFE(varCurrPos, tmp2, &(currGroup->var_head.list)) {
            estats_var* currVar = ESTATS_LIST_ENTRY(varCurrPos, estats_var, list);
            _estats_list_del(varCurrPos); /* Must be before free! */
            free(currVar);
        }

        _estats_list_del(groupCurrPos); /* Must be before free! */
        free(currGroup);
    }

    ESTATS_LIST_FOREACH_SAFE(connCurrPos, tmp, &((*agent)->connection_head.list)) {
        estats_connection* currConn = ESTATS_LIST_ENTRY(connCurrPos, estats_connection, list);
        _estats_list_del(connCurrPos); /* Must be before free! */
        free(currConn);
    }
    
    switch ((*agent)->type) {
    case ESTATS_AGENT_TYPE_LOCAL:
        _estats_agent_detach_local(agent);
        break;
    case ESTATS_AGENT_TYPE_LOG:
        _estats_agent_detach_log(agent);
        break;
    default:
        break;
    }
    
    free(*agent);
    *agent = NULL;
}


estats_error*
estats_agent_get_connection_head(estats_connection** conn, estats_agent* agent)
{
    estats_error* err = NULL;
    struct estats_list* head;

    ErrIf(conn == NULL || agent == NULL, ESTATS_ERR_INVAL);
    Chk(_estats_agent_refresh_connections(agent)); 
    head = &(agent->connection_head.list);
    *conn = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_connection, list);

Cleanup:
    return err;
}


estats_error*
estats_agent_get_group_head(estats_group** group, estats_agent* agent)
{
    estats_error* err = NULL;
    struct estats_list* head;

    ErrIf(group == NULL || agent == NULL, ESTATS_ERR_INVAL);
    head = &(agent->group_head.list);
    *group = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_group, list);

Cleanup:
    return err;
}


estats_error*
estats_agent_get_type(ESTATS_AGENT_TYPE* type, const estats_agent* agent)
{
    estats_error* err = NULL;

    ErrIf(type == NULL || agent == NULL, ESTATS_ERR_INVAL);
    *type = agent->type;
 
 Cleanup:
    return err;
}


estats_error*
estats_agent_get_version(const char** version, const estats_agent *agent)
{
    estats_error* err = NULL;

    ErrIf(version == NULL || agent == NULL, ESTATS_ERR_INVAL);
    *version = agent->version;

Cleanup:
    return err;
}


estats_error*
estats_agent_foreach_connection(estats_agent* agent, estats_connection_foreach_func f, void* userData)
{
    estats_error* err = NULL;
    struct estats_list* head;
    struct estats_list* currItem;

    ErrIf(agent == NULL || f == NULL, ESTATS_ERR_INVAL);
    
    Chk(_estats_agent_refresh_connections(agent));
    
    head = &(agent->connection_head.list);
    ESTATS_LIST_FOREACH(currItem, head) {
        int flags = 0;
        estats_connection* currConn = ESTATS_LIST_ENTRY(currItem, estats_connection, list);

        if (currItem->prev == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_FIRST;
        if (currItem->next == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_LAST;
        
        if (f(currConn, flags, userData) == ESTATS_FOREACH_STOP)
            break;
    }

Cleanup:
    return err;
}


estats_error*
estats_agent_foreach_group(estats_agent* agent, estats_group_foreach_func f, void* userData)
{
    estats_error* err = NULL;
    struct estats_list* head;
    struct estats_list* currItem;

    ErrIf(agent == NULL || f == NULL, ESTATS_ERR_INVAL);

    head = &(agent->group_head.list);
    ESTATS_LIST_FOREACH(currItem, head) {
        int flags = 0;
        estats_group* currGroup = ESTATS_LIST_ENTRY(currItem, estats_group, list);

        if (currItem->prev == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_FIRST;
        if (currItem->next == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_LAST;
        
        if (f(currGroup, flags, userData) == ESTATS_FOREACH_STOP)
            break;
    }

Cleanup:
    return err;
}


estats_error*
estats_agent_find_connection_from_cid(estats_connection** conn,
                                      estats_agent *agent,
                                      const int cid)
{
    estats_error* err = NULL;
    struct estats_list* currItem;
   
    ErrIf(conn == NULL || agent == NULL, ESTATS_ERR_INVAL);
    *conn = NULL;
    
    Chk(_estats_agent_refresh_connections(agent));
    ESTATS_LIST_FOREACH(currItem, &(agent->connection_head.list)) {
        estats_connection* currConn = ESTATS_LIST_ENTRY(currItem, estats_connection, list);
        if (currConn->cid == cid) {
            *conn = currConn;
            break;
        }
    }
    
    ErrIf(*conn == NULL, ESTATS_ERR_NOCONNECTION);

Cleanup:
    return err;
}


estats_error*
estats_agent_find_connection_from_spec(estats_connection** conn,
                                        estats_agent* agent,
                                        const struct estats_connection_spec* spec)
{
    estats_error* err = NULL;
    struct estats_list* currItem;
    int res;
   
    ErrIf(conn == NULL || agent == NULL || spec == NULL, ESTATS_ERR_INVAL);

    *conn = NULL;
    
    Chk(_estats_agent_refresh_connections(agent)); 
    ESTATS_LIST_FOREACH(currItem, &(agent->connection_head.list)) {
        estats_connection* currConn = ESTATS_LIST_ENTRY(currItem, estats_connection, list);
	Chk(estats_value_compare(&res, currConn->spec.dst_port, spec->dst_port));
	if (res) continue;
	Chk(estats_value_compare(&res, currConn->spec.dst_addr, spec->dst_addr));
	if (res) continue;
	Chk(estats_value_compare(&res, currConn->spec.src_port, spec->src_port));
	if (res) continue;
	Chk(estats_value_compare(&res, currConn->spec.src_addr, spec->src_addr));
	if (res) continue;
	break;
    }
   
    ErrIf(*conn == NULL, ESTATS_ERR_NOCONNECTION);

Cleanup:
    return err;
}


estats_error*
estats_agent_find_connection_from_socket(estats_connection** conn,
                                         estats_agent *agent,
                                         int sockfd)
{
    estats_error* err = NULL;
    struct sockaddr_in6 ne6, fe6; /* near and far ends */
    socklen_t namelen; /* may not be POSIX */
    struct estats_connection_spec spec; /* connection tuple */

    /* XXX TODO XXX: Should we only allow local agents? */
    ErrIf(conn == NULL || agent == NULL, ESTATS_ERR_INVAL);
   
    /* refresh_conections is not necessary */

    namelen = sizeof (fe6);
    Chk(Getpeername(sockfd, (struct sockaddr*) &fe6, &namelen));

    namelen = sizeof (ne6);
    Chk(Getsockname(sockfd, (struct sockaddr*) &ne6, &namelen));
    
    switch (((struct sockaddr*) &fe6)->sa_family) {
    case AF_INET:
    {
        struct sockaddr_in *ne4 = (struct sockaddr_in *)&ne6;
        struct sockaddr_in *fe4 = (struct sockaddr_in *)&fe6;
	uint16_t nport = ntohs(ne4->sin_port);
	uint16_t fport = ntohs(fe4->sin_port);

	Chk(estats_value_new(&spec.src_addr, &ne4->sin_addr.s_addr, ESTATS_VALUE_TYPE_IP4ADDR));
	Chk(estats_value_new(&spec.src_port, &nport, ESTATS_VALUE_TYPE_UINT16));
	Chk(estats_value_new(&spec.dst_addr, &fe4->sin_addr.s_addr, ESTATS_VALUE_TYPE_IP4ADDR));
	Chk(estats_value_new(&spec.dst_port, &fport, ESTATS_VALUE_TYPE_UINT16));

        Chk(estats_agent_find_connection_from_spec(conn, agent, &spec));
        break;
    }
    case AF_INET6:
    {
	uint16_t nport = ntohs(ne6.sin6_port);
       	uint16_t fport = ntohs(fe6.sin6_port); 

	Chk(estats_value_new(&spec.src_addr, &ne6.sin6_addr, ESTATS_VALUE_TYPE_IP6ADDR));
	Chk(estats_value_new(&spec.src_port, &nport, ESTATS_VALUE_TYPE_UINT16));
	Chk(estats_value_new(&spec.dst_addr, &fe6.sin6_addr, ESTATS_VALUE_TYPE_IP6ADDR));
	Chk(estats_value_new(&spec.dst_port, &fport, ESTATS_VALUE_TYPE_UINT16));

        Chk(estats_agent_find_connection_from_spec(conn, agent, &spec));
        break;
    }
    default:
        Err(ESTATS_ERR_SOCK);
        break;
    }

Cleanup:
    Free((void**) &spec.src_addr);
    Free((void**) &spec.src_port);
    Free((void**) &spec.dst_addr);
    Free((void**) &spec.dst_port); 

    return err;
}


estats_error*
estats_agent_find_group_from_name(estats_group** group,
                                  const estats_agent* agent,
                                  const char* groupName)
{
    estats_error* err = NULL;
    struct estats_list* currItem;
    
    ErrIf(group == NULL || agent == NULL || groupName == NULL, ESTATS_ERR_INVAL);

    *group = NULL;

    ESTATS_LIST_FOREACH(currItem, &(agent->group_head.list)) {
        estats_group* currGroup = ESTATS_LIST_ENTRY(currItem, estats_group, list);
        if (strcmp(currGroup->name, groupName) == 0) {
            *group = currGroup;
            break;
        }
    }
    
    ErrIf(*group == NULL, ESTATS_ERR_NOGROUP);

 Cleanup:
    return err;
}


estats_error*
estats_agent_find_var_and_group(estats_var** var, estats_group** group,
                                estats_agent* agent, const char* varname)
{
    estats_error* err = NULL;
    struct estats_list* head;
    struct estats_list* currItem;
    estats_var* v = NULL;
    estats_group* g = NULL;
    
    ErrIf(var == NULL || agent == NULL || varname == NULL, ESTATS_ERR_INVAL);
    
    head = &(agent->group_head.list);
    ESTATS_LIST_FOREACH(currItem, head) {
        g = ESTATS_LIST_ENTRY(currItem, estats_group, list);

        if ((err = estats_group_find_var_from_name(&v, g, varname)) == NULL) {
            dbgprintf("Found variable \"%s\" in group \"%s\"\n", varname, g->name);
            CHECK_VAR(v);
            break;
        } else {
            dbgprintf("Ignoring variable \"%s\" not found, checking next group\n", varname);
            estats_error_free(&err);
            v = NULL;
            g = NULL;
        }
    }

    ErrIf(v == NULL || g == NULL, ESTATS_ERR_NOVAR);

    *var = v;
    if (group != NULL)
        *group = g;

 Cleanup:
    return err;
}

    
static estats_error*
_estats_agent_refresh_connections(estats_agent* agent)
{
    estats_error* err = NULL;
    struct dirent* ent;
    DIR* dir = NULL;
    struct estats_list* connHead;
    struct estats_list* connCurrPos;
    struct estats_list* tmp;
    estats_connection* cp = NULL;
    estats_group* spec_gp;
    estats_var* var;
    estats_value* val = NULL;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);
    
    connHead = &(agent->connection_head.list);
    ESTATS_LIST_FOREACH_SAFE(connCurrPos, tmp, connHead) {
        estats_connection* currConn = ESTATS_LIST_ENTRY(connCurrPos, estats_connection, list);
        _estats_list_del(connCurrPos); /* Must be before free! */
        free(currConn);
    }
   
    Chk(Opendir(&dir, ESTATS_ROOT_DIR));
    
    while ((ent = readdir(dir))) {
        int cid;
        const char* addr_name;
        const char* port_name;
        int16_t* dst;
        
        cid = atoi(ent->d_name);
        if (cid == 0 && ent->d_name[0] != '0')
            continue;
       
        Chk(Malloc((void**) &cp, sizeof(estats_connection)));
        cp->agent = agent;
        cp->cid = cid; 

        spec_gp = agent->spec;

        if ((err = estats_group_find_var_from_name(&var, spec_gp, "LocalAddressType")) != NULL) {
            cp->addrtype = ESTATS_ADDRTYPE_IPV4;
            estats_error_free(&err);
        } else {
            Chk(estats_connection_read_value(&val, cp, var));
            Chk(estats_value_as_int32((int32_t*) &(cp->addrtype), val));
            estats_value_free(&val);
        }
        
        if (strncmp(agent->version, "1.", 2) == 0) {
            addr_name = "RemoteAddress";
            port_name = "RemotePort";
        } else {
            addr_name = "RemAddress";
            port_name = "RemPort";
        }

        Chk(estats_group_find_var_from_name(&var, spec_gp, "LocalAddress"));
	Chk(estats_connection_read_value(&cp->spec.src_addr, cp, var));

        Chk(estats_group_find_var_from_name(&var, spec_gp, addr_name)); 
	Chk(estats_connection_read_value(&cp->spec.dst_addr, cp, var));

        Chk(estats_group_find_var_from_name(&var, spec_gp, "LocalPort"));
	Chk(estats_connection_read_value(&cp->spec.src_port, cp, var));

        Chk(estats_group_find_var_from_name(&var, spec_gp, port_name));
	Chk(estats_connection_read_value(&cp->spec.dst_port, cp, var));

        _estats_list_add_tail(&(cp->list), connHead);
        cp = NULL;
    }
    
Cleanup:
    estats_value_free(&val);
    Free((void**) &cp);
    Closedir(&dir);

    return err;
}


static estats_error*
_estats_agent_attach_local(estats_agent** agent)
{
    estats_error* err = NULL;
    FILE* header = NULL;

    ErrIf(agent == NULL, ESTATS_ERR_INVAL);

    (*agent)->type = ESTATS_AGENT_TYPE_LOCAL;

    Chk(Fopen(&header, ESTATS_HEADER_FILE, "r"));
    Chk(_estats_agent_parse_header(*agent, header));

 Cleanup:
    Fclose(&header);
    
    if (err != NULL) {
        _estats_agent_detach_local(agent);
    }
    
    return err;
}


static estats_error*
_estats_agent_attach_log(estats_agent** agent, FILE* header)
{
    estats_error* err = NULL;

//    Chk(Malloc((void**) agent, sizeof(estats_agent)));
    (*agent)->type = ESTATS_AGENT_TYPE_LOG;
//    _estats_list_init(&((*agent)->group_head.list));
//    _estats_list_init(&((*agent)->connection_head.list));
    Chk(_estats_agent_parse_header(*agent, header));

Cleanup:
    return err;
}


static void
_estats_agent_detach_local(estats_agent** agent)
{
}


static void
_estats_agent_detach_log(estats_agent** agent)
{
}
