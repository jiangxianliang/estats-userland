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
    estats_log* log = NULL;
    estats_var* var;
//    struct estats_list* data_list;
//    struct estats_list* pos;
    estats_log_data* hdata;
    estats_log_data* ldata;
    argv0 = argv[0];

    if (argc != 2) {
        usage();
        exit(EXIT_FAILURE);
    }

/* read from log file */

    Chk(estats_log_open(&log, "./log.example", "r"));

//    Chk(estats_log_data_read(&data_list, log));

    Chk(estats_log_find_var_from_name(&var, log, argv[1]));

    Chk(estats_log_get_data_head(&hdata, log));

//    ESTATS_LIST_FOREACH(pos, data_list) {
    ESTATS_LOG_DATA_FOREACH(ldata, hdata) {
//        estats_log_data* ldata = LOG_DATA_ENTRY(pos);
        estats_value* value = NULL;
        char* res;

        Chk(estats_log_data_read_value(&value, ldata, var));
        Chk(estats_value_as_string(&res, value));
        printf("Value is %s\n", res);
        
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

