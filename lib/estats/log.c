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

static estats_error* _estats_log_new(estats_log** _log);
static estats_error* _estats_log_open_read(estats_log* _log, const char* _path);
static estats_error* _estats_log_open_write(estats_log* _log, const char* _path);
static estats_error* _estats_log_calculate_checksum(estats_log* _log, uint16_t* _sum);
static estats_error* _estats_log_write_checksum(estats_log* _log);
static void          _estats_log_free(estats_log** _log);

estats_error*
estats_log_open(estats_log** log, const char* path, const char* mode)
{
    estats_error* err = NULL;

    Chk(_estats_log_new(log));

    switch (mode[0]) {
       	case 'r':
	    Chk(_estats_log_open_read(*log, path));
	    break;
       	case 'w':
	    Chk(_estats_log_open_write(*log, path));
	    break;
       	default:
	    Err(ESTATS_ERR_INVAL);
	    break; /* not reached */
    }

Cleanup:
    if (err != NULL) {
	Free((void**) log);
    }

    return err;
}

estats_error*
estats_log_close(estats_log** log)
{
    estats_error* err = NULL;

    ErrIf(log == NULL || *log == NULL, ESTATS_ERR_INVAL);

    if ((*log)->mode == W_MODE) {
        Chk(_estats_log_write_checksum(*log));
    }

Cleanup:
    _estats_log_free(log);

    return err;
}


static estats_error*
_estats_log_entry_new(estats_log_entry** entry, estats_log* log)
{
    estats_error* err = NULL;

    Chk(Malloc((void**) entry, sizeof(estats_log_entry)));
    memset((void*) *entry, 0, sizeof(estats_log_entry));

    Chk(Malloc((void**) &((*entry)->data), log->bufsize));

    (*entry)->log = log;

Cleanup:

    return err;
}

static void
_estats_log_entry_free(estats_log_entry** entry)
{
    Free(&((*entry)->data));
    Free((void**) entry);
}

static estats_error*
_estats_log_entry_read(estats_log* log)
{
    estats_error* err = NULL;

    ErrIf(log == NULL, ESTATS_ERR_INVAL);
    ErrIf(log->mode != R_MODE, ESTATS_ERR_ACCESS);
    ErrIf(log->fp == NULL, ESTATS_ERR_FILE);

    while (1) {
        estats_log_entry* entry = NULL;

        Chk(_estats_log_entry_new(&entry, log));

        if ((err = Fread(NULL, entry->data, log->bufsize, 1, log->fp)) != NULL) {
    
            if (estats_error_get_number(err) == ESTATS_ERR_EOF) {
                dbgprintf("   ... caught expected EOF at %s:%d in function %s\n", __FILE__, __LINE__, __FUNCTION__);

                estats_error_free(&err);
            }
            _estats_log_entry_free(&entry);
            break;
        }

        _estats_list_add_tail(&(entry->list), &(log->entry_list_head));
    }

Cleanup:

    return err;
}

estats_error*
estats_log_entry_write(estats_log* log, estats_snapshot* snap)
{
    estats_error* err = NULL;

    ErrIf(log == NULL || snap == NULL, ESTATS_ERR_INVAL);
    ErrIf(log->mode != W_MODE, ESTATS_ERR_ACCESS);
    ErrIf(log->fp == NULL, ESTATS_ERR_FILE);

    Chk(Fwrite(NULL, snap->data, snap->group->size, 1, log->fp));

Cleanup:

    return err;
}

estats_error*
estats_log_find_var_from_name(estats_var** var,
                              const estats_log* log,
                              const char* name)
{
    estats_error* err = NULL;
    struct estats_list* lpos;
    
    ErrIf(var == NULL || log == NULL || name == NULL, ESTATS_ERR_INVAL);

    *var = NULL;
    
    ESTATS_LIST_FOREACH(lpos, &(log->var_list_head)) {
        estats_var* currVar = ESTATS_LIST_ENTRY(lpos, estats_var, list);
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

static void
_estats_swap_buf(char* buf, const estats_var* var)
{
    if (var == NULL) return;

    switch (var->type) {
    case ESTATS_TYPE_INTEGER:
    case ESTATS_TYPE_INTEGER32:
    case ESTATS_TYPE_COUNTER32:
    case ESTATS_TYPE_GAUGE32:
    case ESTATS_TYPE_UNSIGNED32:
    case ESTATS_TYPE_TIME_TICKS:
        *(uint32_t*)buf = bswap_32(*(uint32_t*)buf);
        break;
    case ESTATS_TYPE_COUNTER64:
        *(uint64_t*)buf = bswap_64(*(uint64_t*)buf);
        break;
    case ESTATS_TYPE_INET_PORT_NUMBER:
        *(uint16_t*)buf = bswap_16(*(uint16_t*)buf);
        break;
    default: // all other types already handled
        break;
    }

    return;
}

estats_error*
estats_log_entry_read_value(estats_value** value,
                           const estats_log_entry* entry,
                           const estats_var* var)
{
    estats_error* err = NULL;
    char* buf = NULL;
    size_t size = var->len;

    ErrIf(value == NULL || entry == NULL || var == NULL, ESTATS_ERR_INVAL);

    Chk(Malloc((void**) &buf, size));
    memcpy(buf, (void *)((unsigned long int)(entry->data) + var->offset), size);

    if (entry->log->swap) _estats_swap_buf(buf, var);

    Chk(_estats_value_from_var_buf(value, buf, var->type));

Cleanup:
    Free((void**) &buf);

    return err;
}

estats_error*
estats_log_get_entry_head(estats_log_entry** entry, estats_log* log)
{
    estats_error* err = NULL;
    struct estats_list* head;

    ErrIf(entry == NULL || log == NULL, ESTATS_ERR_INVAL);

    head = &(log->entry_list_head);
    *entry = _estats_list_empty(head) ? NULL : ESTATS_LIST_ENTRY(head->next, estats_log_entry, list);

Cleanup:
    return err;
}

estats_error*
estats_log_entry_next(estats_log_entry** next, const estats_log_entry* prev)
{
    estats_error* err = NULL;
    struct estats_list* l;

    ErrIf(next == NULL || prev == NULL, ESTATS_ERR_INVAL);

    l = prev->list.next;
    if (l == &(prev->log->entry_list_head))
        *next = NULL;
    else
        *next = ESTATS_LIST_ENTRY(l, estats_log_entry, list);

Cleanup:
    return err;
}

estats_log_entry*
estats_log_entry_return_next(const estats_log_entry* prev)
{
    estats_error* err = NULL;
    estats_log_entry* next = NULL;

    Chk(estats_log_entry_next(&next, prev));

Cleanup:
    if (err) return NULL;
    else return next;
}

static estats_error*
_estats_log_new(estats_log** log)
{
    estats_error* err = NULL;

    ErrIf(log == NULL, ESTATS_ERR_INVAL);
    *log = NULL;

    Chk(Malloc((void**) log, sizeof(estats_log)));
    memset((void*) *log, 0, sizeof(estats_log));

    _estats_list_init(&((*log)->var_list_head));
    _estats_list_init(&((*log)->entry_list_head));

Cleanup:
    return err;
}

static estats_error* _estats_log_parse_header(estats_log* _log, FILE* _fp);

static estats_error*
_estats_log_open_read(estats_log* log, const char* path)
{
    estats_error* err = NULL;
    FILE* header = NULL;
    uint16_t h_siz;
    uint16_t sum;
    int c;
    uint8_t end;

    ErrIf(log == NULL, ESTATS_ERR_INVAL);

    Chk(Fopen(&(log->fp), path, "r"));

    Chk(_estats_log_calculate_checksum(log, &sum));
    ErrIf(sum != 0, ESTATS_ERR_CHKSUM);

    Chk(Fread(NULL, &end, 1, 1, log->fp));

    Chk(Fread(NULL, &h_siz, 2, 1, log->fp));
    if (end != LOG_HOST_ORDER) {
        log->swap = 1;
        h_siz = bswap_16(h_siz);
    }

    Chk(Fopen(&header, "./log_header", "w+"));

    while (h_siz-- > 0) {
        c = fgetc(log->fp);
       	Chk(Fputc(c, header));
    }

    Chk(Fseek(header, 0, SEEK_SET)); // rewind

    Chk(_estats_log_parse_header(log, header));

    log->mode = R_MODE;

    Chk(_estats_log_entry_read(log));

Cleanup: 
    Fclose(&header);
    Remove("./log_header");

    if (err != NULL) {
        _estats_log_free(&log);
    }
        
    return err;
}

static estats_error*
_estats_log_open_write(estats_log* log, const char* path)
{
    estats_error* err = NULL; 
    FILE* header = NULL;
    int c;
    uint16_t h_siz = 0;
    uint8_t end = LOG_HOST_ORDER;
    fpos_t pos;

    ErrIf(log == NULL, ESTATS_ERR_INVAL);

    Chk(Fopen(&log->fp, path, "w+")); // need to read back for checksum calc 

    Chk(Fwrite(NULL, &end, 1, 1, log->fp)); // indicate endianness of write

    Chk(Fwrite(NULL, &h_siz, 2, 1, log->fp)); // place holder for size of header

    Chk(Fopen(&header, ESTATS_HEADER_FILE, "r"));

    while ((c = fgetc(header)) != EOF) {
        Chk(Fputc(c, log->fp));
        h_siz++;
    }

    ErrIf(fgetpos(log->fp, &pos) != 0, ESTATS_ERR_LIBC);

    Chk(Fseek(log->fp, 1, SEEK_SET)); // rewind to write size of header

    Chk(Fwrite(NULL, &h_siz, 2, 1, log->fp));

    ErrIf(fsetpos(log->fp, &pos) != 0, ESTATS_ERR_LIBC);

    log->mode = W_MODE;

Cleanup:
    Fclose(&header);

    if (err != NULL) {
        _estats_log_free(&log);
    }

    return err;
}

static estats_error*
_estats_log_calculate_checksum(estats_log* log, uint16_t* csum)
{
    estats_error* err = NULL;
    uint8_t* buf = NULL;
    uint16_t word16;
    uint32_t sum;
    int pad = 0;
    struct stat st;
    off_t size;
    int i;

    Chk(Fseek(log->fp, 0, SEEK_SET));

    fstat(fileno(log->fp), &st);
    size = st.st_size;

    Chk(Calloc((void**) &buf, size+1, sizeof(uint8_t)));

    Chk(Fread(NULL, buf, size, 1, log->fp));

    if ((size | 1) == size) {
	pad = 1;
    }

    sum = 0;
    for (i=0; i<size+pad; i=i+2) {
       	word16 =((buf[i] << 8) & 0xFF00) + (buf[i+1] & 0x00FF); 
	sum = sum + (uint32_t) word16;
    }

    while (sum>>16) {
       	sum = (sum & 0xFFFF) + (sum >> 16);
    }

    word16 = sum;
    if (pad) word16 = ((word16 & 0x00FF) << 8) + ((word16 & 0xFF00) >> 8);

    *csum = (~word16) & 0xFFFF;

Cleanup:
    Free((void**) &buf); 
    Fseek(log->fp, 0, SEEK_SET);

    return err;
}


static estats_error*
_estats_log_write_checksum(estats_log* log)
{
    estats_error* err = NULL;
    uint16_t sum;
    uint8_t sum1, sum2;

    ErrIf(log == NULL || log->fp == NULL || log->mode != W_MODE, ESTATS_ERR_INVAL);

    Chk(_estats_log_calculate_checksum(log, &sum));

    sum1 = (sum & 0xFF00) >> 8;
    sum2 = sum & 0xFF;

    Chk(Fseek(log->fp, 0, SEEK_END));

    Chk(Fwrite(NULL, &sum1, sizeof(uint8_t), 1, log->fp));
    Chk(Fwrite(NULL, &sum2, sizeof(uint8_t), 1, log->fp));

Cleanup:
    
    return err;
}

static void
_estats_log_free(estats_log** log)
{
    struct estats_list* pos;
    struct estats_list* tmp;

    if (log == NULL || *log == NULL) return;

    Fclose(&(*log)->fp);

    ESTATS_LIST_FOREACH_SAFE(pos, tmp, &((*log)->entry_list_head)) {
        estats_log_entry* entry = ESTATS_LIST_ENTRY(pos, estats_log_entry, list);
        _estats_list_del(pos);
        _estats_log_entry_free(&entry);
    }

    ESTATS_LIST_FOREACH_SAFE(pos, tmp, &((*log)->var_list_head)) {
        estats_var* var = ESTATS_LIST_ENTRY(pos, estats_var, list);
        _estats_list_del(pos);
        free(var);
    }

    Free((void**)log);
}

estats_error*
_estats_log_parse_header(estats_log* log, FILE* fp)
{
    estats_error* err = NULL;
    estats_var* var = NULL;
    char version[ESTATS_VERSTR_LEN_MAX];
    char group_name[ESTATS_GROUPNAME_LEN_MAX];
    char linebuf[256];
    int have_len = 0;

    ErrIf(log == NULL || fp == NULL, ESTATS_ERR_INVAL);

    /* First line is version string */
    Chk(Fgets(linebuf, sizeof(linebuf), fp));
    strlcpy(version, linebuf, sizeof(version));

    if (strncmp(version, "1.", 2) != 0)
        have_len = 1;

    while (1) {
        size_t len;
        
        if ((err = Fgets(linebuf, sizeof(linebuf), fp)) != NULL) {
            if (estats_error_get_number(err) == ESTATS_ERR_EOF) {
                dbgprintf("   ... caught expected EOF at %s:%d in function %s\n", __FILE__, __LINE__, __FUNCTION__);
                estats_error_free(&err);
                break;
            } else {
                goto Cleanup;
            }
        }

        /* Trim out trailing \n if it exists */
        len = strlen(linebuf);
        if (len > 0 && linebuf[len - 1] == '\n')
            linebuf[len - 1] = '\0';

        /* Skip over blank lines */
        if (strisspace(linebuf))
            continue;

        if (linebuf[0] == '/') {
            strlcpy(group_name, linebuf + 1, sizeof(group_name));
            dbgprintf("New group: %s\n", group_name);
        } else {
            int nRead;
            int fsize;

            if (strcmp(group_name, "read") != 0) continue;

            log->bufsize = 0;
            log->nvars = 0;
            
            /* Add a new variable */
            Chk(Malloc((void**) &var, sizeof(estats_var)));
            memset((void*) var, 0, sizeof(estats_var));

            Chk(Sscanf(&nRead, linebuf, "%s%d%d%d", var->name, &var->offset, &var->type, &var->len));
            ErrIf(nRead != 4, ESTATS_ERR_HEADER);

            /* Deprecated variable check */
            var->flags = 0;
            if (var->name[0] == '_') {
                var->flags |= ESTATS_VAR_FL_DEP;
                memmove(var->name, var->name + 1, strlen(var->name)); /* Purposely move the NULL byte */
            }

#if defined(DEBUG)
            dbgprintf("    New ");
            if (var->flags & ESTATS_VAR_FL_DEP)
                dbgprintf_no_prefix("(deprecated) ");
            dbgprintf_no_prefix("var: %s offset=%d type=%d len=%d\n", var->name, var->offset, var->type, var->len);
#endif /* defined(DEBUG) */

            /* increment group (== file) size if necessary */ 
            fsize = var->offset + var->len;
            log->bufsize = ((log->bufsize < fsize) ? fsize : log->bufsize); 

            log->nvars++;
            _estats_list_add_tail(&(var->list), &(log->var_list_head));
            var = NULL;
        }
    }

Cleanup:
    if (err != NULL) {
       	Free((void**) &var);
    }

    return err;
}


