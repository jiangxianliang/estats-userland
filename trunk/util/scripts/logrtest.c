/*
 *
 */
#include "scripts.h"

static const char* argv0 = NULL;

static void
usage(void)
{
    fprintf(stderr,
            "Usage: %s <variable name>\n",
            argv0);
}

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_connection* conn;
    estats_group* group;
    estats_var* var;
//    estats_snapshot* snap = NULL;
    estats_snapshot* snapold = NULL;
    estats_snapshot* snapnew = NULL;
    estats_value* value = NULL;
    estats_log* log = NULL;
    int cid;
    char* text = NULL;
    int eof = 0;
    int ii;

    argv0 = argv[0];

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    cid = atoi(argv[1]);

/* read from log file */

    Chk(estats_log_open(&log, "./log.example", "r"));
    Chk(estats_log_get_agent(&agent, log));
    Chk(estats_agent_find_var_and_group(&var, &group, agent, argv[1]));

    Chk(estats_log_snapshot_alloc(&snapold, log));
    Chk(estats_log_snapshot_alloc(&snapnew, log));

//    Chk(estats_log_snap(snapold, log));
    Chk(estats_log_snap(snapnew, log));

    Chk(estats_snapshot_read_value(&value, snapnew, var));
    Chk(estats_value_as_string(&text, value));
    
    printf("Initial value of %s: %s\n", argv[1], text);

    free(text);
    estats_value_free(&value);
    
    ii=0;
    while (1) {
	sleep(1);

        SWAP(snapnew, snapold);
        Chk(estats_log_snap(snapnew, log));

	estats_log_eof(&eof, log);
        printf("iter = %d; eof = %d\n", ii++, eof);
	if (eof) break;

        Chk(estats_snapshot_read_value(&value, snapnew, var));
        Chk(estats_value_as_string(&text, value));
        printf("New value of %s: %s\n", argv[1], text);

        free(text);
        estats_value_free(&value);

        Chk(estats_snapshot_delta(&value, snapnew, snapold, var));
        Chk(estats_value_as_string(&text, value));

        printf("Change in %s: %s\n", argv[1], text);

        free(text);
        estats_value_free(&value);
    }

Cleanup:
//    estats_snapshot_free(&snap);
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

