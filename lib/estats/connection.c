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


estats_error*
estats_connection_next(estats_connection** next, const estats_connection* prev)
{
    estats_error* err = NULL;
    struct estats_list* l;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);
    ErrIf(prev->agent->type != ESTATS_AGENT_TYPE_LOCAL, ESTATS_ERR_AGENT_TYPE);

    l = prev->list.next;
    if (l == &(prev->agent->connection_list_head))
        *next = NULL;
    else
        *next = ESTATS_LIST_ENTRY(l, estats_connection, list);

Cleanup:
    return err;
}

estats_connection*
estats_connection_return_next(const estats_connection* prev)
{
    estats_error* err = NULL;
    estats_connection* next = NULL;

    Chk(estats_connection_next(&next, prev));

Cleanup:
    if (err) return NULL;
    else return next;
}

estats_error*
estats_connection_get_agent(estats_agent** agent, const estats_connection* connection)
{
    estats_error* err = NULL;

    ErrIf(agent == NULL || connection == NULL, ESTATS_ERR_INVAL);
    *agent = connection->agent;

Cleanup:
    return err;
}


estats_error*
estats_connection_get_cid(int* cid, const estats_connection* connection)
{
    estats_error* err = NULL;

    ErrIf(cid == NULL || connection == NULL, ESTATS_ERR_INVAL);
    *cid = connection->cid;

Cleanup:
    return err;
}


estats_error*
estats_connection_get_addrtype(ESTATS_ADDRTYPE* addrtype, const estats_connection* connection)
{
    estats_error* err = NULL;

    ErrIf(addrtype == NULL || connection == NULL, ESTATS_ERR_INVAL);
    *addrtype = connection->addrtype;

Cleanup:
    return err;
}

estats_error*
estats_connection_get_connection_spec(struct estats_connection_spec *spec, const estats_connection* conn)
{
    estats_error* err = NULL;

    ErrIf(spec == NULL || conn == NULL, ESTATS_ERR_INVAL);

    *spec = conn->spec; // struct copy

Cleanup:
    return err;
}

estats_error*
estats_connection_spec_addr_as_string(char** str, const char* addr)
{
    estats_error* err = NULL;
    estats_value* val = NULL;

    Chk(_estats_value_from_var_buf(&val, (void*) addr, ESTATS_TYPE_INET_ADDRESS));
    Chk(estats_value_as_string(str, val));

 Cleanup:
    estats_value_free(&val);
    return err;
}


estats_error* estats_connection_spec_as_strings(struct spec_ascii* spec_asc, struct estats_connection_spec* spec)
{
    estats_error* err = NULL;
    estats_value* val = NULL;

    spec_asc->dst_port = NULL;
    spec_asc->src_port = NULL;

    Chk(_estats_value_from_var_buf(&val, (void*) &spec->dst_addr, ESTATS_TYPE_INET_ADDRESS));
    Chk(estats_value_as_string(&spec_asc->dst_addr, val));
    estats_value_free(&val);

    Chk(Asprintf(NULL, &spec_asc->dst_port, "%u", spec->dst_port));

    Chk(_estats_value_from_var_buf(&val, (void*) &spec->src_addr, ESTATS_TYPE_INET_ADDRESS));
    Chk(estats_value_as_string(&spec_asc->src_addr, val));
    estats_value_free(&val);

    Chk(Asprintf(NULL, &spec_asc->src_port, "%u", spec->src_port));

 Cleanup:
    if (err != NULL) {
        estats_value_free(&val);
        Free((void**) &spec_asc->dst_port);
        Free((void**) &spec_asc->src_port);
    }
    return err;
}


estats_error*
estats_connection_read_access(const estats_connection* conn,
                              int mode)
{
    estats_error* err = NULL;
    char* filename = NULL;

    ErrIf(conn == NULL, ESTATS_ERR_INVAL);
    Chk(Asprintf(NULL, &filename, "%s/%d/read", ESTATS_ROOT_DIR, conn->cid));
    Chk(Access(filename, mode));

 Cleanup:
    Free((void**) &filename);
    return err;
}


estats_error*
estats_connection_read_value(estats_value** value,
                             const estats_connection* conn,
                             const estats_var* var)
{
    estats_error* err = NULL;
    int size;
    char* buf = NULL;
    char* filename = NULL;
    FILE* fp = NULL;
    
    ErrIf(value == NULL || conn == NULL || var == NULL, ESTATS_ERR_INVAL);
    ErrIf(var->group == NULL, ESTATS_ERR_INVAL);
    ErrIf(var->group->agent != conn->agent, ESTATS_ERR_INVAL);
    ErrIf(conn->agent == NULL, ESTATS_ERR_INVAL);
    ErrIf(conn->agent->type != ESTATS_AGENT_TYPE_LOCAL, ESTATS_ERR_AGENT_TYPE);

    Chk(_estats_var_size_from_type(&size, var->type));
    Chk(Malloc((void**) &buf, size));
    
    Chk(Asprintf(NULL, &filename, "%s/%d/%s", ESTATS_ROOT_DIR, conn->cid, var->group->name));
    Chk(Fopen(&fp, filename, "r"));
    Chk(Fseek(fp, var->offset, SEEK_SET));
    Chk(Fread(NULL, buf, size, 1, fp));

    Chk(_estats_value_from_var_buf(value, buf, var->type));

 Cleanup:
    Fclose(&fp);
    Free((void**) &filename);
    Free((void**) &buf);

    return err;
}


estats_error*
estats_connection_write_value(const estats_value* value,
                              const estats_connection* conn,
                              const estats_var* var)
{
    estats_error* err = NULL;
    void* buf = NULL;
    size_t size;
    char* filename = NULL;
    FILE* fp = NULL;
    
    ErrIf(value == NULL || conn == NULL || var == NULL, ESTATS_ERR_INVAL);
    ErrIf(var->group == NULL, ESTATS_ERR_INVAL);
    ErrIf(var->group->agent != conn->agent, ESTATS_ERR_INVAL);
    ErrIf(conn->agent == NULL, ESTATS_ERR_INVAL);
    ErrIf(conn->agent->type != ESTATS_AGENT_TYPE_LOCAL, ESTATS_ERR_AGENT_TYPE);

    Chk(_estats_value_to_var_buf(&buf, &size, value, var->type));

    Chk(Asprintf(NULL, &filename, "%s/%d/%s", ESTATS_ROOT_DIR, conn->cid, var->group->name));
    Chk(Fopen(&fp, filename, "w"));
    Chk(Fseek(fp, var->offset, SEEK_SET));
    Chk(Fwrite(NULL, buf, size, 1, fp));

 Cleanup:
    Fclose(&fp);
    Free((void**) &filename);
    Free((void**) &buf);

    return err;
}


estats_error*
estats_connection_spec_compare(int* res,
                               const struct estats_connection_spec *s1,
                               const struct estats_connection_spec *s2)
{
    estats_error* err = NULL;

    ErrIf(s1 == NULL || s2 == NULL, ESTATS_ERR_INVAL);

    *res = 1;

    if ( (s1->dst_port == s2->dst_port) &&
         strcmp(s1->dst_addr, s2->dst_addr) == 0 &&
         (s1->src_port == s2->src_port) &&
         strcmp(s1->src_addr, s2->src_addr) == 0 ) {

        *res = 0;
    }

Cleanup:
    return err;
}




