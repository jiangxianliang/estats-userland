#include "scripts.h"

int main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_var* var_head;
    estats_var* var_pos;
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

            Chk(estats_agent_get_var_head(&var_head, agent));

            ESTATS_VAR_FOREACH(var_pos, var_head) {
                const char* varName;
                estats_value* value = NULL;
                char* text = NULL;

                Chk(estats_var_get_name(&varName, var_pos));
                Chk(estats_connection_read_value(&value, conn, var_pos));
                Chk(estats_value_as_string(&text, value));

                fprintf(stdout, "    %-20s: %s\n", varName, text);

                free(text);
                estats_value_free(&value);
            }
    }

 Cleanup:
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
