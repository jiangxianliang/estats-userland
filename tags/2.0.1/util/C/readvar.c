/*
 * readvar: Reads the current value for one or more estats variables in
 *          a connection.  Estats variables are separated by spaces.
 *
 * Usage: readvar <connection id> <var name> [<var name> ...]
 * Example: readvar 1359 LocalPort LocalAddress
 *
 */

#include "scripts.h"

static const char* argv0 = NULL;

static void
usage(void)
{
    fprintf(stderr,
            "Usage: %s <connection id> <var name> [<var name> ...]\n",
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
    char** arg;

    argv0 = argv[0];

    if (argc < 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    cid = atoi(argv[1]);
   
    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
    Chk(estats_agent_find_connection_from_cid(&conn, agent, cid));
   
    for (arg=&argv[2]; *arg; arg++) {
        char* text = NULL;

        Chk(estats_agent_find_var_from_name(&var, agent, *arg));
        Chk(estats_connection_read_value(&value, conn, var));
        Chk(estats_value_as_string(&text, value));
    
        printf("%-20s: %s\n", *arg, text);

        free(text);
        estats_value_free(&value);
    }

Cleanup:
    estats_value_free(&value);
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}

