/*
 *
 */
#include "scripts.h"

int main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_log* log = NULL;

    Chk(estats_log_open(&log, "./test-log", "w"));
    Chk(estats_log_close(&log));

    Chk(estats_log_open(&log, "./test-log", "r"));
    Chk(estats_log_close(&log));

Cleanup:

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

