#include <estats/estats-int.h>


estats_error*
estats_connection_next(estats_connection** next, const estats_connection* prev)
{
    estats_error* err = NULL;
    struct estats_list* l;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);
    ErrIf(prev->agent->type != ESTATS_AGENT_TYPE_LOCAL, ESTATS_ERR_AGENT_TYPE);

    l = prev->list.next;
    if (l == &(prev->agent->connection_head.list))
        *next = NULL;
    else
        *next = ESTATS_LIST_ENTRY(l, estats_connection, list);

Cleanup:
    return err;
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
estats_connection_get_spec(struct estats_connection_spec *spec, const estats_connection* conn)
{
    estats_error* err = NULL;

    ErrIf(spec == NULL || conn == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_value_copy(&spec->dst_port, conn->spec.dst_port));
    Chk(_estats_value_copy(&spec->dst_addr, conn->spec.dst_addr));
    Chk(_estats_value_copy(&spec->src_port, conn->spec.src_port));
    Chk(_estats_value_copy(&spec->src_addr, conn->spec.src_addr));

Cleanup:
    return err;
}


estats_error*
estats_connection_group_access(const estats_connection* conn,
                               const estats_group* group,
                               int mode)
{
    estats_error* err = NULL;
    char* filename = NULL;

    ErrIf(conn == NULL || group == NULL, ESTATS_ERR_INVAL);
    Chk(Asprintf(NULL, &filename, "%s/%d/%s", ESTATS_ROOT_DIR, conn->cid, group->name));
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
estats_connection_spec_compare(int* result,
                               const struct estats_connection_spec *s1,
                               const struct estats_connection_spec *s2)
{
    estats_error* err = NULL;
    int sa, sp, da, dp;
//    ESTATS_ADDRTYPE type1;
//    ESTATS_ADDRTYPE type2;

    ESTATS_VALUE_TYPE type1;
    ESTATS_VALUE_TYPE type2;

    ErrIf(s1 == NULL || s2 == NULL, ESTATS_ERR_INVAL);

    Chk(estats_value_get_type(&type1, s1->src_addr));
    Chk(estats_value_get_type(&type2, s2->src_addr));

    if (type1 != type2) {
	*result = 1;
	return;
    }

    Chk(estats_value_compare(&sa, s1->src_addr, s2->src_addr));
    Chk(estats_value_compare(&sp, s1->src_port, s2->src_port));
    Chk(estats_value_compare(&da, s1->dst_addr, s2->dst_addr));
    Chk(estats_value_compare(&dp, s1->dst_port, s2->dst_port));

    if (!sa && !sp && !da && !dp) *result = 0;
    else *result = 1;

Cleanup:

    return err;
}


estats_error*
estats_connection_spec_copy(struct estats_connection_spec* s1,
                      const struct estats_connection_spec* s2)
{
    estats_error* err = NULL;

    ErrIf(s1 == NULL || s2 == NULL, ESTATS_ERR_INVAL);

    Chk(_estats_value_copy(&s1->src_addr, s2->src_addr));
    Chk(_estats_value_copy(&s1->src_port, s2->src_port));
    Chk(_estats_value_copy(&s1->dst_addr, s2->dst_addr));
    Chk(_estats_value_copy(&s1->dst_port, s2->dst_port));

Cleanup:

    return err;
}











