
#include "scripts.h"

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_group* group;

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
    Chk(estats_agent_get_group_head(&group, agent));
    while (group != NULL) {
        estats_var* var;

        Chk(estats_group_get_var_head(&var, group));
        while (var != NULL) {
            const char* name;

            Chk(estats_var_get_name(&name, var));
            printf("%s\n", name);
            Chk(estats_var_next(&var, var));
        }

        Chk(estats_group_next(&group, group));
    }
    
 Cleanup:
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
