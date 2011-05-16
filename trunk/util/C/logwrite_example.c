/*
 *
 */
#include "scripts.h"
#include <time.h>

static const char* argv0 = NULL;

static void
usage(void)
{
    fprintf(stderr,
            "Usage: %s <connection id>\n",
            argv0);
}

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_connection* conn;
    estats_snapshot* snap = NULL;
    estats_log* log = NULL;
    int cid;
    int ii;

    argv0 = argv[0];

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    cid = atoi(argv[1]);

/* initialize and write 5 atomic reads of estats vars to log */

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
    Chk(estats_agent_find_connection_from_cid(&conn, agent, cid));

    Chk(estats_log_open(&log, "./log.example", "w"));

    Chk(estats_snapshot_alloc(&snap, conn)); 
    for (ii = 0; ii < 5; ii++) {
        struct estats_timeval etv;
        char* str_time;
        time_t c_time;

        Chk(estats_take_snapshot(snap));
	Chk(estats_log_write_entry(log, snap));
        Chk(estats_snapshot_get_timeval(&etv, snap));
        c_time = (time_t) etv.sec;
        str_time = ctime(&c_time);
        printf("Recording snapshot at %s (usec %u)\n", str_time, etv.usec); 
	sleep(1);
    }
    estats_log_close(&log);

Cleanup:
    estats_snapshot_free(&snap);
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

