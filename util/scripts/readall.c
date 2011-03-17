
#include "scripts.h"

static ESTATS_FOREACH_RET group_handler(estats_group* group, int flags, void* userData);
static ESTATS_FOREACH_RET conn_handler(estats_connection* conn, int flags, void* userData);
static ESTATS_FOREACH_RET var_handler(estats_var* var, int flags, void* userData);

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
            char* srcAddr = NULL;
            char* dstAddr = NULL;
            char* srcPort = NULL;
            char* dstPort = NULL;
    estats_group* grp_head;
    estats_group* grp_pos;
    estats_connection* c_head;
    estats_connection* c_pos;
    const char* groupName;
    const char* varName;
    
    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));

    Chk(estats_agent_get_group_head(&grp_head, agent));
    ESTATS_GROUP_FOREACH(grp_pos, grp_head) {

        Chk(estats_group_get_name(&groupName, grp_pos));
        fprintf(stdout, "Group \"%s\"\n", groupName);

        Chk(estats_agent_get_connection_head(&c_head, agent));
        ESTATS_CONNECTION_FOREACH(c_pos, c_head) {
            estats_var* var_head;
            estats_var* var_pos;
            struct estats_connection_spec spec; 
            int cid;

            /* If no permission to access group, ignore and go to next connection */
            if (estats_connection_group_access(c_pos, grp_pos, R_OK)) continue;

//    if (!(flags & ESTATS_FOREACH_FLAGS_IS_FIRST))
//        fputc('\n', stdout);

            Chk(estats_connection_get_cid(&cid, c_pos));
            Chk(estats_connection_get_spec(&spec, c_pos));

            Chk(estats_value_as_string(&srcAddr, spec.src_addr));
            Chk(estats_value_as_string(&srcPort, spec.src_port));
            Chk(estats_value_as_string(&dstAddr, spec.dst_addr));
            Chk(estats_value_as_string(&dstPort, spec.dst_port));

            fprintf(stdout, "Connection %d (%s:%s %s:%s)\n", cid, srcAddr, srcPort, dstAddr, dstPort);

            Chk(estats_group_get_var_head(&var_head, grp_pos));
            ESTATS_VAR_FOREACH(var_pos, var_head) {
                estats_value* value = NULL;
                char* text = NULL;

                Chk(estats_var_get_name(&varName, var_pos));
                Chk(estats_connection_read_value(&value, c_pos, var_pos));
                Chk(estats_value_as_string(&text, value));

                fprintf(stdout, "    %-20s: %s\n", varName, text);

                free(text);
                estats_value_free(&value);
            }
            free(srcAddr);
            free(dstAddr);
            free(srcPort);
            free(dstPort);
        }
    }

Cleanup:
    estats_agent_detach(&agent);
/*
    if (srcAddr) free(srcAddr);
    if (dstAddr) free(dstAddr);
    if (srcPort) free(srcPort);
    if (dstPort) free(dstPort);
*/
    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static ESTATS_FOREACH_RET
group_handler(estats_group* group, int flags, void* userData)
{
    estats_error* err = NULL;
    const char* groupName;

    if (!(flags & ESTATS_FOREACH_FLAGS_IS_FIRST))
        fputc('\n', stdout);

    Chk(estats_group_get_name(&groupName, group));
    fprintf(stdout, "Group \"%s\"\n", groupName);
    Chk(estats_agent_foreach_connection((estats_agent*) userData, conn_handler, group));

Cleanup:
    if (err != NULL) {
        PRINT_AND_FREE(err);
        return ESTATS_FOREACH_STOP;
    }

    return ESTATS_FOREACH_CONTINUE;
}

static ESTATS_FOREACH_RET
conn_handler(estats_connection* conn, int flags, void* userData)
{
    estats_error* err = NULL;
    int cid;
    struct estats_connection_spec spec; 
    char* srcAddr = NULL;
    char* dstAddr = NULL;
    char* srcPort = NULL;
    char* dstPort = NULL;

    /* If no permission to access group, ignore and go to next connection */
    ChkIgn(estats_connection_group_access(conn, (estats_group*) userData, R_OK));

    if (!(flags & ESTATS_FOREACH_FLAGS_IS_FIRST))
        fputc('\n', stdout);

    Chk(estats_connection_get_cid(&cid, conn));
    Chk(estats_connection_get_spec(&spec, conn));

    Chk(estats_value_as_string(&srcAddr, spec.src_addr));
    Chk(estats_value_as_string(&srcPort, spec.src_port));
    Chk(estats_value_as_string(&dstAddr, spec.dst_addr));
    Chk(estats_value_as_string(&dstPort, spec.dst_port));

    fprintf(stdout, "Connection %d (%s:%s %s:%s)\n", cid, srcAddr, srcPort, dstAddr, dstPort);
    Chk(estats_group_foreach_var((estats_group*) userData, var_handler, conn));

Cleanup:
    if (srcAddr) free(srcAddr);
    if (dstAddr) free(dstAddr);
    if (srcPort) free(srcPort);
    if (dstPort) free(dstPort);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return ESTATS_FOREACH_STOP;
    }

    return ESTATS_FOREACH_CONTINUE;
}

static ESTATS_FOREACH_RET
var_handler(estats_var* var, int flags, void* userData)
{
    estats_error* err = NULL;
    const char* varName;
    estats_value* value = NULL;
    char* text = NULL;

    Chk(estats_var_get_name(&varName, var));
    Chk(estats_connection_read_value(&value, (estats_connection*) userData, var));
    Chk(estats_value_as_string(&text, value));

    fprintf(stdout, "    %-20s: %s\n", varName, text);

Cleanup:
    free(text);
    estats_value_free(&value);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return ESTATS_FOREACH_STOP;
    }

    return ESTATS_FOREACH_CONTINUE;
}
