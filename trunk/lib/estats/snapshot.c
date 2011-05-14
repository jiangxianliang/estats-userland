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
estats_snapshot_alloc(estats_snapshot** snap, estats_connection* conn)
{
    estats_error* err = NULL;
    int size;

    ErrIf(snap == NULL, ESTATS_ERR_INVAL);
    *snap = NULL;

    ErrIf(conn == NULL, ESTATS_ERR_INVAL);

    size = conn->agent->read->size;

    Chk(Malloc((void**) snap, sizeof(estats_snapshot)));
    Chk(Malloc((void**) &((*snap)->data), size));

    memset((*snap)->data, 0, size);

    (*snap)->group = conn->agent->read;
    (*snap)->cid = conn->cid;
    (*snap)->spec = conn->spec; // struct copy

Cleanup:
    if (err != NULL) {
        estats_snapshot_free(snap);
    }

    return err;
}


void
estats_snapshot_free(estats_snapshot** snap)
{
    if (snap == NULL || *snap == NULL)
        return;

    Free((void**) &((*snap)->data));
    Free((void**) snap);
}

estats_error*
_estats_gettimeofday(struct estats_timeval* etv)
{
    estats_error* err = NULL;
    struct timeval tv;

    ErrIf((gettimeofday(&tv, NULL) != 0), ESTATS_ERR_LIBC);
    etv->sec = (uint32_t) (tv.tv_sec & 0x00000000ffffffffULL);
    etv->usec = (uint32_t) (tv.tv_usec & 0x00000000ffffffffULL);

Cleanup:
    return 0;
}

estats_error*
estats_take_snapshot(estats_snapshot* snap)
{
    estats_error* err = NULL;
    FILE *fp = NULL;
    char filename[PATH_MAX];
    
    ErrIf(snap == NULL, ESTATS_ERR_INVAL);
    ErrIf(snap->group == NULL, ESTATS_ERR_INVAL);
    ErrIf(snap->group->agent == NULL, ESTATS_ERR_INVAL);
    ErrIf(snap->group->agent->type != ESTATS_AGENT_TYPE_LOCAL, ESTATS_ERR_AGENT_TYPE);

    snprintf(filename, PATH_MAX - 1, "%s/%d/%s", ESTATS_ROOT_DIR, snap->cid, snap->group->name);
    Chk(Fopen(&fp, filename, "r"));
    Chk(Fread(NULL, snap->data, snap->group->size, 1, fp)); 
    Chk(_estats_gettimeofday(&snap->tv)); // for rough chronology
   
Cleanup:
    Fclose(&fp);
    
    return err;
}


estats_error*
estats_snapshot_read_value(estats_value** value,
                           const estats_snapshot* snap,
                           const estats_var* var)
{
    estats_error* err = NULL;
    char* buf = NULL;
    
    ErrIf(value == NULL || snap == NULL || var == NULL, ESTATS_ERR_INVAL);
    ErrIf(var->group != snap->group, ESTATS_ERR_INVAL);

    Chk(Malloc((void**) &buf, var->len));
    memcpy(buf, (void *)((unsigned long)(snap->data) + var->offset), var->len);

    Chk(_estats_value_from_var_buf(value, buf, var->type));

Cleanup:
    Free((void**) &buf);
    
    return err;
}


estats_error*
estats_snapshot_delta(estats_value** value,
                      const estats_snapshot* s1,
                      const estats_snapshot* s2,
                      const estats_var* var)
{
    estats_error* err = NULL;
    estats_value* v1 = NULL;
    estats_value* v2 = NULL;

    ErrIf(value == NULL || s1 == NULL || s2 == NULL || var == NULL, ESTATS_ERR_INVAL);
    ErrIf(s1->group != s2->group, ESTATS_ERR_INVAL);

    Chk(estats_snapshot_read_value(&v1, s1, var));
    Chk(estats_snapshot_read_value(&v2, s2, var));
    Chk(estats_value_difference(value, v1, v2));

Cleanup:
    estats_value_free(&v2);
    estats_value_free(&v1);
    
    return err;
}

estats_error*
estats_snapshot_get_timeval(struct estats_timeval* etv, estats_snapshot* snap)
{
    estats_error* err = NULL;

    ErrIf(etv == NULL || snap == NULL, ESTATS_ERR_INVAL);

    *etv = snap->tv;

Cleanup:
    return err;
}


