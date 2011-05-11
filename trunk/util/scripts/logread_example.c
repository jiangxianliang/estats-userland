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
    estats_log_entry* entry = NULL;

    argv0 = argv[0];

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

    Chk(estats_log_open(&log, "./log.example", "r"));

    Chk(estats_log_find_var_from_name(&var, log, argv[1]));

    while ((err = estats_log_read_entry(&entry, log)) == NULL) {

        struct estats_timeval etv;
        estats_value* value = NULL;
        char* res;
        char* str_time;
        time_t c_time;

        Chk(estats_log_entry_read_value(&value, entry, var));
        Chk(estats_value_as_string(&res, value));
        printf("Value is %s, ", res);
        
        Chk(estats_log_entry_read_timestamp(&etv, entry));
        c_time = (time_t) etv.sec;
        str_time = ctime(&c_time);
        printf("recorded at %s\n", str_time);

        free(res);
        estats_value_free(&value);
        estats_log_entry_free(&entry);
    }

    if (estats_error_get_number(err) == ESTATS_ERR_EOF) estats_error_free(&err);

    Chk(estats_log_close(&log));

Cleanup:

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

