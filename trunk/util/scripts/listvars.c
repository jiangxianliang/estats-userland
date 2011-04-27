
#include "scripts.h"

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));

    estats_var* var;

    Chk(estats_agent_get_var_head(&var, agent));
    while (var != NULL) {
        const char* name;

        Chk(estats_var_get_name(&name, var));
        printf("%s\n", name);
        Chk(estats_var_next(&var, var));
    }

Cleanup:
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
