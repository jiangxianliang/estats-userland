#include "scripts.h"

static char* type_array[13];
static char* value_type_array[8];
static void value_type_from_var_type(const char **, ESTATS_TYPE);

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
        ESTATS_TYPE var_type;
        const char* val_type;

        Chk(estats_var_get_name(&name, var));
        Chk(estats_var_get_type(&var_type, var));
        value_type_from_var_type(&val_type, var_type);
        printf("%-23s %-30s %s\n", name, type_array[var_type], val_type);
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

static char* type_array[13] = {
    "ESTATS_TYPE_INTEGER",
    "ESTATS_TYPE_INTEGER32",
    "ESTATS_TYPE_INET_ADDRESS_IPV4",
    "ESTATS_TYPE_COUNTER32",
    "ESTATS_TYPE_GAUGE32",
    "ESTATS_TYPE_UNSIGNED32",
    "ESTATS_TYPE_TIME_TICKS",
    "ESTATS_TYPE_COUNTER64",
    "ESTATS_TYPE_INET_PORT_NUMBER",
    "ESTATS_TYPE_INET_ADDRESS",
    "ESTATS_TYPE_INET_ADDRESS_IPV6",
    "DUMMY",
    "ESTATS_TYPE_OCTET"
};

static char* value_type_array[8] = {
    "ESTATS_VALUE_TYPE_UINT16",
    "ESTATS_VALUE_TYPE_UINT32",
    "ESTATS_VALUE_TYPE_UINT64",
    "ESTATS_VALUE_TYPE_IP4ADDR or ESTATS_VALUE_TYPE_IP6ADDR",
    "ESTATS_VALUE_TYPE_OCTET"
};

static void value_type_from_var_type(const char** val_type, ESTATS_TYPE var_type)
{
    switch (var_type) {
    case ESTATS_TYPE_INTEGER:
    case ESTATS_TYPE_INTEGER32:
    case ESTATS_TYPE_COUNTER32:
    case ESTATS_TYPE_GAUGE32:
    case ESTATS_TYPE_UNSIGNED32:
    case ESTATS_TYPE_TIME_TICKS:
        *val_type = value_type_array[1];
        break;
    case ESTATS_TYPE_COUNTER64:
        *val_type = value_type_array[2];
        break;
    case ESTATS_TYPE_INET_PORT_NUMBER:
        *val_type = value_type_array[0];
        break;
    case ESTATS_TYPE_INET_ADDRESS_IPV4:
    case ESTATS_TYPE_INET_ADDRESS_IPV6:
    case ESTATS_TYPE_INET_ADDRESS:
        *val_type = value_type_array[3];
        break;
    case ESTATS_TYPE_OCTET:
        *val_type = value_type_array[4];
        break;
    default:
        break;
    }
}
