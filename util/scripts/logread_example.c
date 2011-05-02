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
            "Usage: %s <variable name>\n",
            argv0);
}

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_log* log = NULL;
    estats_var* var;
    estats_log_entry* entry_head;
    estats_log_entry* entry_pos;

    argv0 = argv[0];

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

/* read from log file */

    Chk(estats_log_open(&log, "./log.example", "r"));

    Chk(estats_log_find_var_from_name(&var, log, argv[1]));

    Chk(estats_log_get_entry_head(&entry_head, log));

    ESTATS_LOG_DATA_FOREACH(entry_pos, entry_head) {
        struct estats_timeval etv;
        estats_value* value = NULL;
        char* res;
        char* str_time;
        time_t c_time;

        Chk(estats_log_entry_read_value(&value, entry_pos, var));
        Chk(estats_value_as_string(&res, value));
        printf("Value is %s, ", res);
        
        Chk(estats_log_entry_read_timestamp(&etv, entry_pos));
        c_time = (time_t) etv.sec;
        str_time = ctime(&c_time);
        printf("recorded at %s\n", str_time);

        free(res);
        estats_value_free(&value);
    }

    Chk(estats_log_close(&log));

Cleanup:

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

