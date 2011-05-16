#include "scripts.h"

static const char* argv0 = NULL;

static void
usage(void)
{
    fprintf(stderr,
            "Usage: %s <connection id> <var name>\n",
            argv0);
}

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_connection* conn;
    estats_var* var;
    estats_snapshot* snapold = NULL;
    estats_snapshot* snapnew = NULL;
    estats_value* value = NULL;
    char* text = NULL;
    int cid;

    argv0 = argv[0];

    if (argc != 3) {
        usage();
        exit(EXIT_FAILURE);
    }

    cid = atoi(argv[1]);

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
    Chk(estats_agent_find_connection_from_cid(&conn, agent, cid)); 

    Chk(estats_agent_find_var_from_name(&var, agent, argv[2]));

    Chk(estats_snapshot_alloc(&snapold, conn)); 
    Chk(estats_snapshot_alloc(&snapnew, conn));

    Chk(estats_take_snapshot(snapold));
    Chk(estats_take_snapshot(snapnew));

    Chk(estats_snapshot_read_value(&value, snapold, var));
    Chk(estats_value_as_string(&text, value));
    
    printf("Initial value of %s: %s\n", argv[2], text);

    free(text);
    estats_value_free(&value);
    
    while (1) {
        sleep(1);

        SWAP(snapnew, snapold);
        Chk(estats_take_snapshot(snapnew));
        Chk(estats_snapshot_delta(&value, snapnew, snapold, var));
        Chk(estats_value_as_string(&text, value));

        printf("Change in %s: %s\n", argv[2], text);

        free(text);
        estats_value_free(&value);
    }

Cleanup:
    estats_snapshot_free(&snapnew);
    estats_snapshot_free(&snapold);
    estats_value_free(&value);
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

