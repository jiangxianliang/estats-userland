/*
 * Copyright (c) 2011 The Board of Trustees of the University of Illinois,
 *                    Carnegie Mellon University.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; either version 2.1 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 *
 */
#include <estats/estats-int.h>


estats_error*
_estats_group_get_var_head(estats_var** var, estats_group* group)
{
    estats_error* err = NULL;
    struct estats_list* head = NULL;

    ErrIf(var == NULL || group == NULL, ESTATS_ERR_INVAL);
    ErrIf(group->agent == NULL, ESTATS_ERR_INVAL);

    head = &(group->var_list_head);
    *var = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_var, list);
    if ((*var)->flags & ESTATS_VAR_FL_DEP)
        Chk(_estats_var_next_undeprecated(var, *var));

Cleanup:
    return err;
}


estats_error*
_estats_group_find_var_from_name(estats_var** var, const estats_group* group, const char* name)
{
    estats_error* err = NULL;
    struct estats_list* currItem;
    
    ErrIf(var == NULL || group == NULL || name == NULL, ESTATS_ERR_INVAL);
    ErrIf(group->agent == NULL, ESTATS_ERR_INVAL);

    *var = NULL;
    
    ESTATS_LIST_FOREACH(currItem, &(group->var_list_head)) {
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
