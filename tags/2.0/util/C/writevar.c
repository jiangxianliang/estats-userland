
#include "scripts.h"

static const char* argv0 = NULL;

static void
usage(void)
{
    fprintf(stderr,
            "Usage: %s <connection id> <var name> <value>\n",
            argv0);
}

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_connection* conn;
    estats_var* var;
    estats_value* value = NULL;
    int cid;

    argv0 = argv[0];

    if (argc != 4) {
        usage();
        exit(EXIT_FAILURE);
    }

    cid = atoi(argv[1]);
   
    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
    Chk(estats_agent_find_connection_from_cid(&conn, agent, cid)); 

    Chk(estats_agent_find_var_from_name(&var, agent, argv[2])); 

    Chk(estats_value_new(&value, argv[3], ESTATS_VALUE_TYPE_STRING));
    Chk(estats_connection_write_value(value, conn, var));

Cleanup:
    estats_value_free(&value);
    estats_agent_detach(&agent);
    
    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
