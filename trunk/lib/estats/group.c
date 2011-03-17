#include <estats/estats-int.h>


estats_error*
estats_group_next(estats_group** next, const estats_group* prev)
{
    estats_error* err = NULL;
    struct estats_list* l;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    l = prev->list.next;
    if (l == &(prev->agent->group_head.list))
        *next = NULL;
    else
        *next = ESTATS_LIST_ENTRY(l, estats_group, list);

Cleanup:
    return err;
}

estats_group*
estats_group_next_utility(const estats_group* prev)
{
    estats_error* err = NULL;
    estats_group* next = NULL;

    Chk(estats_group_next(&next, prev));

Cleanup:
    if (err) return NULL;
    else return next;
}

estats_error*
estats_group_get_var_head(estats_var** var, estats_group* group)
{
    estats_error* err = NULL;

    ErrIf(var == NULL || group == NULL, ESTATS_ERR_INVAL);
    ErrIf(group->agent == NULL, ESTATS_ERR_INVAL);

//    *var = NULL;

    Chk(_estats_var_next_undeprecated(var, &(group->var_head)));

Cleanup:
    return err;
}


estats_error*
estats_group_get_agent(estats_agent** agent, const estats_group* group)
{
    estats_error* err = NULL;

    ErrIf(agent == NULL || group == NULL, ESTATS_ERR_INVAL);
    *agent = group->agent;
    
Cleanup:
    return err;
}


estats_error*
estats_group_get_name(const char** name,
                      const estats_group* group)
{
    estats_error* err = NULL;

    ErrIf(name == NULL || group == NULL, ESTATS_ERR_INVAL);
    *name = group->name;
    
 Cleanup:
    return err;
}


estats_error*
estats_group_get_size(int* size,
                      const estats_group* group)
{
    estats_error* err = NULL;

    ErrIf(size == NULL || group == NULL, ESTATS_ERR_INVAL);
    *size = group->size;

 Cleanup:
    return err;
}


estats_error*
estats_group_get_nvars(int* nvars,
                       const estats_group* group)
{
    estats_error* err = NULL;

    ErrIf(nvars == NULL || group == NULL, ESTATS_ERR_INVAL);
    *nvars = group->nvars;

 Cleanup:
    return err;
}


estats_error*
estats_group_foreach_var(estats_group* group, estats_var_foreach_func f, void* userData)
{
    estats_error* err = NULL;
    struct estats_list* head;
    estats_var* var;
    
    ErrIf(group == NULL || f == NULL, ESTATS_ERR_INVAL);

    head = &(group->var_head.list);
    Chk(_estats_var_next_undeprecated(&var, &(group->var_head)));

    while (var != NULL) {
        int flags = 0;
        struct estats_list* iter = &(var->list);

        if (iter->prev == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_FIRST;
        if (iter->next == head)
            flags |= ESTATS_FOREACH_FLAGS_IS_LAST;
        
        if (f(var, flags, userData) == ESTATS_FOREACH_STOP)
            break;

        Chk(_estats_var_next_undeprecated(&var, var));
    }

Cleanup:
    return err;
}


estats_error*
estats_group_find_var_from_name(estats_var** var, const estats_group* group, const char* name)
{
    estats_error* err = NULL;
    struct estats_list* currItem;
    
    ErrIf(var == NULL || group == NULL || name == NULL, ESTATS_ERR_INVAL);
    ErrIf(group->agent == NULL, ESTATS_ERR_INVAL);

    *var = NULL;
    
    ESTATS_LIST_FOREACH(currItem, &(group->var_head.list)) {
        estats_var* currVar = ESTATS_LIST_ENTRY(currItem, estats_var, list);
        if (strcmp(currVar->name, name) == 0) {
            *var = currVar;
            break;
        }
    }

    Err2If(*var == NULL, ESTATS_ERR_NOVAR, name);
    CHECK_VAR(*var);

Cleanup:
    return err;
}
