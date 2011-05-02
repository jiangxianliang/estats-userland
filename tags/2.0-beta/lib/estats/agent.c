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

static estats_error* _estats_agent_refresh_connections(estats_agent *agent);
static estats_error* _estats_agent_attach_local(estats_agent** agent);


static estats_error*
_estats_agent_parse_header(estats_agent* agent, FILE* fp)
{
    estats_error* err = NULL;
    estats_group* group = NULL;
    estats_group* curr_gp = NULL; /* DO NOT FREE */
    estats_var* var = NULL;
    char linebuf[256];

    ErrIf(agent == NULL || fp == NULL, ESTATS_ERR_INVAL);

    /* First line is version string */
    Chk(Fgets(linebuf, sizeof(linebuf), fp));
    strlcpy(agent->version, linebuf, sizeof(agent->version));

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
            _estats_list_init(&(group->var_list_head));

            strlcpy(group->name, linebuf + 1, sizeof(group->name));

            dbgprintf("New group: %s\n", group->name);

            /* Distinguish between the group we are working on and the
               'current' group to use in the var logic.  If we don't do
               this right we may have a double-free scenario. */
            curr_gp = group;

            if (strcmp(group->name, "spec") == 0) {
                agent->spec = group;
                group = NULL;
            } else if (strcmp(group->name, "read") == 0) {
                agent->read = group;
                group = NULL;
            } else Err(ESTATS_ERR_HEADER);
            
        } else {
            int nread;
            int fsize;

            /* Add a new variable */
            ErrIf(curr_gp == NULL, ESTATS_ERR_HEADER);

            Chk(Malloc((void**) &var, sizeof(estats_var)));
            var->group = curr_gp;
            var->offset = 0; var->type = 0; var->len=0;


            Chk(Sscanf(&nread, linebuf, "%s%d%d%d", var->name, &var->offset, &var->type, &var->len));
            ErrIf(nread != 4, ESTATS_ERR_HEADER);

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

            /* increment group (== file) size if necessary */
            fsize = var->offset + var->len;
            curr_gp->size = ((curr_gp->size < fsize) ? fsize : curr_gp->size);

            curr_gp->nvars++;

            _estats_list_add_tail(&(var->list), &(curr_gp->var_list_head));
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
    memset((void*) *agent, 0, sizeof(estats_agent));
    _estats_list_init(&((*agent)->connection_list_head));

    switch (type) {
    case ESTATS_AGENT_TYPE_LOCAL:
       	Chk(_estats_agent_attach_local(agent));
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
    struct estats_list* var_pos;
    struct estats_list* conn_pos;
    struct estats_list* tmp;

    estats_group* group = NULL;

    if (agent == NULL || *agent == NULL)
        return;

    group = (*agent)->spec;
        
    ESTATS_LIST_FOREACH_SAFE(var_pos, tmp, &(group->var_list_head)) {
        estats_var* currVar = ESTATS_LIST_ENTRY(var_pos, estats_var, list);
        _estats_list_del(var_pos); /* Must be before free! */
        free(currVar);
    }

    Free((void**) &group);

    group = (*agent)->read;
        
    ESTATS_LIST_FOREACH_SAFE(var_pos, tmp, &(group->var_list_head)) {
        estats_var* currVar = ESTATS_LIST_ENTRY(var_pos, estats_var, list);
        _estats_list_del(var_pos); /* Must be before free! */
        free(currVar);
    }

    Free((void**) &group);

    ESTATS_LIST_FOREACH_SAFE(conn_pos, tmp, &((*agent)->connection_list_head)) {
        estats_connection* currConn = ESTATS_LIST_ENTRY(conn_pos, estats_connection, list);
        _estats_list_del(conn_pos); /* Must be before free! */
        free(currConn);
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
    head = &(agent->connection_list_head);
    *conn = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_connection, list);

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
estats_agent_find_connection_from_cid(estats_connection** conn,
                                      estats_agent *agent,
                                      const int cid)
{
    estats_error* err = NULL;
    struct estats_list* currItem;
   
    ErrIf(conn == NULL || agent == NULL, ESTATS_ERR_INVAL);
    *conn = NULL;
    
    Chk(_estats_agent_refresh_connections(agent));
    ESTATS_LIST_FOREACH(currItem, &(agent->connection_list_head)) {
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
    struct estats_list* list_pos;
    int res;
   
    ErrIf(conn == NULL || agent == NULL || spec == NULL, ESTATS_ERR_INVAL);

    *conn = NULL;
    
    Chk(_estats_agent_refresh_connections(agent)); 
    ESTATS_LIST_FOREACH(list_pos, &(agent->connection_list_head)) {
        estats_connection* conn_pos = ESTATS_LIST_ENTRY(list_pos, estats_connection, list);

        Chk(estats_connection_spec_compare(&res, &conn_pos->spec, spec));
        if (res == 0) {
            *conn = conn_pos;
            break;
        }
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
    socklen_t namelen;
    struct estats_connection_spec spec;

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

	memcpy(&spec.src_addr, &ne4->sin_addr.s_addr, 4);
	memcpy(&spec.src_port, &nport, 2);
	memcpy(&spec.dst_addr, &fe4->sin_addr.s_addr, 4);
	memcpy(&spec.dst_port, &fport, 2);

        Chk(estats_agent_find_connection_from_spec(conn, agent, &spec));
        break;
    }
    case AF_INET6:
    {
	uint16_t nport = ntohs(ne6.sin6_port);
       	uint16_t fport = ntohs(fe6.sin6_port); 

	memcpy(&spec.src_addr, &ne6.sin6_addr, 16);
	memcpy(&spec.src_port, &nport, 2);
	memcpy(&spec.dst_addr, &fe6.sin6_addr, 16);
	memcpy(&spec.dst_port, &fport, 2);

        Chk(estats_agent_find_connection_from_spec(conn, agent, &spec));
        break;
    }
    default:
        Err(ESTATS_ERR_SOCK);
        break;
    }

Cleanup:
    return err;
}

estats_error*
estats_agent_find_var_from_name(estats_var** var, const estats_agent* agent, const char* name)
{
    estats_error* err = NULL;
    struct estats_list* currItem;
    estats_group* group;
    
    ErrIf(var == NULL || agent == NULL || name == NULL, ESTATS_ERR_INVAL);

    group = agent->read;

    *var = NULL;
    
    ESTATS_LIST_FOREACH(currItem, &(group->var_list_head)) {
        estats_var* currVar = ESTATS_LIST_ENTRY(currItem, estats_var, list);
        if (strcmp(currVar->name, name) == 0) {
            *var = currVar;
            break;
        }
    }

    Err2If(*var == NULL, ESTATS_ERR_NOVAR, name);
    CHECK_VAR(*var);

Cleanup:
    return err;
}

    
estats_error*
estats_agent_get_var_head(estats_var** var, const estats_agent* agent)
{
    estats_error* err = NULL;
    struct estats_list* lp = NULL;

    ErrIf(var == NULL || agent == NULL, ESTATS_ERR_INVAL);
    ErrIf(agent->read == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_group_get_var_head(var, agent->read));

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
    
    connHead = &(agent->connection_list_head);
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
        FILE *fp = NULL;
        char filename[PATH_MAX];
        void* buf = NULL;
        struct in_addr ip4addr_val;
        char* str_val;
        uint32_t addrtype = 0;

        cid = atoi(ent->d_name);
        if (cid == 0 && ent->d_name[0] != '0')
            continue;
       
        Chk(Malloc((void**) &cp, sizeof(estats_connection)));
        cp->agent = agent;
        cp->cid = cid; 
        cp->addrtype = ESTATS_ADDRTYPE_UNKNOWN;

        spec_gp = agent->spec;

        Chk(Sprintf(NULL, filename, "%s/%d/%s", ESTATS_ROOT_DIR, cid, spec_gp->name));
        Chk(Fopen(&fp, filename, "r"));
        Chk(Malloc(&buf, spec_gp->size));
        Chk(Fread(NULL, buf, spec_gp->size, 1, fp));

        Chk(_estats_group_find_var_from_name(&var, spec_gp, "LocalAddressType"));
        memcpy(&addrtype, (void *)((unsigned long int)buf + var->offset), var->len);

        cp->addrtype = addrtype;

        Chk(_estats_group_find_var_from_name(&var, spec_gp, "LocalAddress"));
        memcpy(cp->spec.src_addr, (void *)((unsigned long int)buf + var->offset), var->len);

        Chk(_estats_group_find_var_from_name(&var, spec_gp, "RemAddress")); 
        memcpy(cp->spec.dst_addr, (void *)((unsigned long int)buf + var->offset), var->len);


        Chk(_estats_group_find_var_from_name(&var, spec_gp, "LocalPort"));
        memcpy(&(cp->spec.src_port), (void *)((unsigned long int)buf + var->offset), var->len);


        Chk(_estats_group_find_var_from_name(&var, spec_gp, "RemPort"));
        memcpy(&(cp->spec.dst_port), (void *)((unsigned long int)buf + var->offset), var->len);

        _estats_list_add_tail(&(cp->list), connHead);
        cp = NULL;

        Fclose(&fp);
        Free(&buf);
    }
    
Cleanup:
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

    return err;
}


