#include "scripts.h"

int main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
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
            char* srcAddr = NULL;
            char* dstAddr = NULL;
            char* srcPort = NULL;
            char* dstPort = NULL;
            int cid;

            /* If no permission to access group, ignore and go to next connection */
            if ((err = estats_connection_group_access(c_pos, grp_pos, R_OK)) != NULL) {
                estats_error_free(&err);
                continue;
            }

            Chk(estats_connection_get_cid(&cid, c_pos));
            Chk(estats_connection_get_connection_spec(&spec, c_pos));

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
            estats_value_free(&spec.src_addr);
            estats_value_free(&spec.src_port);
            estats_value_free(&spec.dst_addr);
            estats_value_free(&spec.dst_port);
            free(srcAddr); free(srcPort); free(dstAddr); free(dstPort);
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

