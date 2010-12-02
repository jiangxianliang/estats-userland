#include <estats/estats-int.h>


estats_error*
_estats_var_size_from_type(int* size, ESTATS_TYPE type)
{
    estats_error* err = NULL;
    
    switch (type) {
    case ESTATS_TYPE_INTEGER:
    case ESTATS_TYPE_INTEGER32:
    case ESTATS_TYPE_INET_ADDRESS_IPV4:
    case ESTATS_TYPE_COUNTER32:
    case ESTATS_TYPE_GAUGE32:
    case ESTATS_TYPE_UNSIGNED32:
    case ESTATS_TYPE_TIME_TICKS:
        *size = 4;
        break;
    case ESTATS_TYPE_COUNTER64:
        *size = 8;
        break;
    case ESTATS_TYPE_INET_PORT_NUMBER:
        *size = 2;
        break;
    case ESTATS_TYPE_INET_ADDRESS:
    case ESTATS_TYPE_INET_ADDRESS_IPV6:
        *size = 17;
        break;
    default:
        Err(ESTATS_ERR_UNKNOWN_TYPE);
        break;
    }

 Cleanup:
    return err;
}


void
_estats_var_dep_check(estats_var* var)
{
    if (var->flags & ESTATS_VAR_FL_DEP) {
        if (!(var->flags & ESTATS_VAR_FL_WARNED))
            fprintf(stderr, "libestats: warning: accessing depricated variable %s\n", var->name);
        var->flags |= ESTATS_VAR_FL_WARNED;
    }
}


estats_error*
_estats_var_next_undeprecated(estats_var** next, estats_var* prev)
{
    estats_error* err = NULL;
    struct estats_list* head;
    struct estats_list* iter;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    head = &(prev->group->var_head.list);
    iter = &(prev->list);

    do {
        iter = iter->next;

        /* End of list */
        if (iter == head) {
            *next = NULL;
            break;
        }

        *next = ESTATS_LIST_ENTRY(iter, estats_var, list);
    } while ((*next)->flags & ESTATS_VAR_FL_DEP);

 Cleanup:
    return err;
}


estats_error*
estats_var_next(estats_var** next, estats_var* prev)
{
    return _estats_var_next_undeprecated(next, prev);
}


estats_error*
estats_var_get_name(const char** name, const estats_var* var)
{
    estats_error* err = NULL;

    ErrIf(name == NULL || var == NULL, ESTATS_ERR_INVAL);
    *name = var->name;

Cleanup:
    return err;
}


estats_error*
estats_var_get_type(ESTATS_TYPE* type, const estats_var* var)
{
    estats_error* err = NULL;

    ErrIf(type == NULL || var == NULL, ESTATS_ERR_INVAL);
    *type = var->type;

Cleanup:
    return err;
}


estats_error*
estats_var_get_size(size_t* size, const estats_var* var)
{
    estats_error* err = NULL;

    ErrIf(size == NULL || var == NULL, ESTATS_ERR_INVAL);
    *size = var->len;

Cleanup:
    return err;
}
