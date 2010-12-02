/*
 * TODO:
 * - Add back in ability to optionally read variables only from a certain
 *   group?
 * - Print connection info?
 * - Separate variables into groups?
 */
#include "scripts.h"

static ESTATS_FOREACH_RET group_handler(estats_group* group, int flags, void* userData);
static ESTATS_FOREACH_RET var_handler(estats_var* var, int flags, void* userData);

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_connection* conn;
    int i;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <cid1> [<cid2> ...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
    
    for (i = 1; i < argc; i++) {
        int cid = atoi(argv[i]);
        fprintf(stdout, "Connection %d\n", cid);
        Chk(estats_agent_find_connection_from_cid(&conn, agent, cid));
        Chk(estats_agent_foreach_group(agent, group_handler, conn));
    }

 Cleanup:
    estats_agent_detach(&agent);

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
    
    Chk(estats_group_foreach_var(group, var_handler, userData));

 Cleanup:
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
    estats_connection* conn;
    const char* varName;
    estats_value* value = NULL;
    char* text = NULL;
    
    conn = (estats_connection*) userData;

    Chk(estats_var_get_name(&varName, var));
    Chk(estats_connection_read_value(&value, conn, var));
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
