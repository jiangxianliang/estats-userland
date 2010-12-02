#include <estats/estats-int.h>

#define BEGIN_HEADER_MARKER  "----Begin-Header-----"
#define END_OF_HEADER_MARKER "----End-Of-Header---- -1 -1"
#define BEGIN_SNAP_DATA      "----Begin-Snap-Data----"
#define MAX_TMP_BUF_SIZE     32 
#define ANNOTATION_LEN_MAX   255

static estats_error* _estats_log_open_read(estats_log* log, const char* path);
static void          _estats_log_close_read(estats_log* log);
static estats_error* _estats_log_open_write(estats_log* log, const char* path);
static void          _estats_log_close_write(estats_log* log);
static void          _estats_log_write_annotation(estats_log* log);
static void          _estats_log_write_checksum(estats_log* log);
static estats_error* _estats_log_calculate_checksum(estats_log* log, uint16_t* sum);


static estats_error*
_estats_log_open_read(estats_log* log, const char* path)
{
    estats_error* err = NULL;
    estats_var* var;
    estats_snapshot* snap = NULL;
    estats_agent* agent;
    FILE* header = NULL;
    char tmpbuf[MAX_TMP_BUF_SIZE];
    char annbuf[ANNOTATION_LEN_MAX + 1];
    uint16_t sum;
    fpos_t pos;
    int c; 

    ErrIf(log == NULL, ESTATS_ERR_INVAL);
    log->fp = NULL; 
    log->agent = NULL;
    log->note = NULL;

    Chk(Fopen(&(log->fp), path, "r"));

    Chk(_estats_log_calculate_checksum(log, &sum));
    ErrIf(sum != 0, ESTATS_ERR_CHKSUM);

    Chk(Fread(NULL, annbuf, ANNOTATION_LEN_MAX + 1, 1, log->fp));
    Chk(Strndup(&log->note, annbuf, ANNOTATION_LEN_MAX));

    Chk(Fgets(tmpbuf, MAX_TMP_BUF_SIZE, log->fp));
    ErrIf(strncmp(tmpbuf, BEGIN_HEADER_MARKER, strlen(BEGIN_HEADER_MARKER)) != 0, ESTATS_ERR_FILE);

    Chk(Fopen(&header, "./log_header", "w+"));

    while ((c = fgetc(log->fp)) != '\0') {
       	Chk(Fputc(c, header));
    }

    Chk(Fgets(tmpbuf, MAX_TMP_BUF_SIZE, log->fp));
    ErrIf(strncmp(tmpbuf, END_OF_HEADER_MARKER, strlen(END_OF_HEADER_MARKER)) != 0, ESTATS_ERR_FILE);

    rewind(header);

    Chk(estats_agent_attach(&log->agent, ESTATS_AGENT_TYPE_LOG, header));

    Chk(estats_log_snapshot_alloc(&snap, log));

    Chk(Fgets(tmpbuf, MAX_TMP_BUF_SIZE, log->fp));
    ErrIf(strncmp(tmpbuf, BEGIN_SNAP_DATA, strlen(BEGIN_SNAP_DATA)) != 0, ESTATS_ERR_FILE);

    if (fgetpos(log->fp, &pos))
       	Err(ESTATS_ERR_LIBC);

    Chk(Fread(NULL, snap->data, snap->group->size, 1, log->fp));

    if (fsetpos(log->fp, &pos))
       	Err(ESTATS_ERR_LIBC);

    Chk(estats_group_find_var_from_name(&var, snap->group, "RemAddress"));
    Chk(estats_snapshot_read_value(&log->spec.dst_port, snap, var));

    Chk(estats_group_find_var_from_name(&var, snap->group, "RemPort"));
    Chk(estats_snapshot_read_value(&log->spec.dst_addr, snap, var));

    Chk(estats_group_find_var_from_name(&var, snap->group, "LocalAddress"));
    Chk(estats_snapshot_read_value(&log->spec.src_port, snap, var));

    Chk(estats_group_find_var_from_name(&var, snap->group, "LocalPort"));
    Chk(estats_snapshot_read_value(&log->spec.src_addr, snap, var));

    log->mode = LOG_READ;

Cleanup: 
    Fclose(&header);
    Remove("./log_header");

    estats_snapshot_free(&snap);

    if (err != NULL) {
        _estats_log_close_read(log);
    }
        
    return err;
}

static void
_estats_log_close_read(estats_log* log)
{
    if (log == NULL) return;

    Fclose(&(log->fp));
    estats_agent_detach(&log->agent);
}

static estats_error*
_estats_log_open_write(estats_log* log, const char* path)
{
    estats_error* err = NULL; 
    FILE* header = NULL;
    char ann[ANNOTATION_LEN_MAX];
    int c;

    ErrIf(log == NULL, ESTATS_ERR_INVAL);
    log->fp = NULL; 
    log->note = NULL;

    Chk(Fopen(&log->fp, path, "w+")); // Need to read back for checksum calc 

    memset(ann, 0, ANNOTATION_LEN_MAX);
    Chk(Fwrite(NULL, ann, 1, ANNOTATION_LEN_MAX, log->fp));    
    Chk(Fprintf(NULL, log->fp, "\n"));

    Chk(Fprintf(NULL, log->fp, "%s\n", BEGIN_HEADER_MARKER));

    Chk(Fopen(&header, ESTATS_HEADER_FILE, "r"));
    while ((c = fgetc(header)) != EOF) {
        Chk(Fputc(c, log->fp));
    }
    Chk(Fputc('\0', log->fp));

    Chk(Fprintf(NULL, log->fp, "%s\n", END_OF_HEADER_MARKER));

    log->mode = LOG_WRITE;
    log->first_write = 1;

Cleanup:
    Fclose(&header);

    if (err != NULL) {
        _estats_log_close_write(log);
    }

    return err;
}


static void
_estats_log_close_write(estats_log* log) 
{ 
    if (log == NULL) return;

    Fclose(&(log->fp));
}


static void
_estats_log_write_annotation(estats_log* log)
{ 
    if (log == NULL || log->fp == NULL) return;

    Fseek(log->fp, 0, SEEK_SET);
    if (log->note) fputs(log->note, log->fp);
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

    Chk(Calloc((void**) &buf, size+1, sizeof(uint8_t))); /* of course, a malloc would suffice, but let's be explicit */

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


static void
_estats_log_write_checksum(estats_log* log)
{
    uint16_t sum;
    uint8_t sum1, sum2;

    if (log == NULL || log->fp == NULL) return;

    (void) _estats_log_calculate_checksum(log, &sum);

    sum1 = (sum & 0xFF00) >> 8;
    sum2 = sum & 0xFF;

    Fseek(log->fp, 0, SEEK_END);

    Fwrite(NULL, &sum1, sizeof(uint8_t), 1, log->fp);
    Fwrite(NULL, &sum2, sizeof(uint8_t), 1, log->fp);
}


estats_error*
estats_log_open(estats_log** log, const char* path, const char* mode)
{
    estats_error* err = NULL;

    ErrIf(log == NULL, ESTATS_ERR_INVAL);
    *log = NULL;

    Chk(Malloc((void**) log, sizeof(estats_log)));

    switch (*mode) {
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


void
estats_log_close(estats_log** log)
{ 
    uint16_t sum;

    if (log == NULL || *log == NULL) return;

    switch ((*log)->mode) {
	case LOG_READ:
	    _estats_log_close_read(*log);
	    break;
	case LOG_WRITE:
	    _estats_log_write_annotation(*log);
	    _estats_log_write_checksum(*log);
	    _estats_log_close_write(*log);
	    break;
	default:
	    return;
    }
    Free((void**) &(*log)->note);
    Free((void**) log);
}


estats_error*
estats_log_snapshot_alloc(estats_snapshot** snap, estats_log* log)
{
    estats_error* err = NULL;
    estats_group* group;
   
    ErrIf(snap == NULL, ESTATS_ERR_INVAL);
    *snap = NULL;

    ErrIf(log == NULL, ESTATS_ERR_INVAL);

    Chk(Malloc((void**) snap, sizeof(estats_snapshot)));

    Chk(estats_agent_find_group_from_name(&((*snap)->group), log->agent, "read"));

    Chk(Malloc((void**) &((*snap)->data), (*snap)->group->size));

    memset((*snap)->data, 0, (*snap)->group->size);

Cleanup:
    if (err != NULL) {
        estats_snapshot_free(snap);
    }

    return err;
}


estats_error*
estats_log_snap(estats_snapshot* snap, estats_log* log)
{
    estats_error* err = NULL;
    
    ErrIf(snap == NULL || log == NULL, ESTATS_ERR_INVAL); 
    ErrIf(snap->group == NULL, ESTATS_ERR_INVAL);
    ErrIf(snap->group->agent == NULL, ESTATS_ERR_INVAL);
    ErrIf(snap->group->agent->type != ESTATS_AGENT_TYPE_LOG, ESTATS_ERR_AGENT_TYPE);

    ErrIf(log->mode != LOG_READ, ESTATS_ERR_ACCESS);
    ErrIf(log->fp == NULL, ESTATS_ERR_FILE);

    if ((err = Fread(NULL, snap->data, snap->group->size, 1, log->fp)) != NULL) {
	if (estats_error_get_number(err) == ESTATS_ERR_EOF) {
	   dbgprintf("   ... caught expected EOF at %s:%d in function %s\n", __FILE__, __LINE__, __FUNCTION__);
	  estats_error_free(&err);
       	} 
    }

Cleanup:

    return err;
}


estats_error*
estats_log_write(estats_log* log, const estats_snapshot* snap)
{ 
    estats_error* err = NULL;
    const char* name;
    estats_group* gp;
    struct estats_connection_spec snap_spec;
    int res;

    ErrIf(log == NULL || snap == NULL, ESTATS_ERR_INVAL);
    ErrIf(log->mode != LOG_WRITE, ESTATS_ERR_ACCESS);
    ErrIf(log->fp == NULL, ESTATS_ERR_FILE);

    Chk(estats_snapshot_get_group_name(&name, snap));
    ErrIf(strcmp(name, "read"), ESTATS_ERR_INVAL);

    if (log->first_write == 1) {
	Chk(estats_connection_spec_copy(&log->spec, &snap->spec));
       	Chk(Fprintf(NULL, log->fp, "%s\n", BEGIN_SNAP_DATA));
	log->first_write = 0;
    }

    Chk(estats_connection_spec_compare(&res, &log->spec, &snap->spec));
    ErrIf(res, ESTATS_ERR_INVAL);

    Chk(Fwrite(NULL, snap->data, snap->group->size, 1, log->fp));

Cleanup:
    return err;
}


estats_error*
estats_log_get_agent(estats_agent** agent, const estats_log* log)
{
    estats_error* err = NULL;

    ErrIf(agent == NULL || log == NULL, ESTATS_ERR_INVAL);

    *agent = log->agent;

Cleanup:
    return err;
}

estats_error*
estats_log_get_mode(int* mode, const estats_log* log)
{
    estats_error* err = NULL;

    *mode = (int) log->mode;

Cleanup:
    return err;
}

estats_error*
estats_log_get_spec(struct estats_connection_spec* spec, const estats_log* log)
{ 
    estats_error* err = NULL;

    ErrIf(spec == NULL || log == NULL, ESTATS_ERR_INVAL);

    if (log->mode == LOG_WRITE && log->first_write == 1) { // log->spec not yet recorded
	spec = NULL;
	return;
    }

    Chk(_estats_value_copy(&spec->dst_port, log->spec.dst_port));
    Chk(_estats_value_copy(&spec->dst_addr, log->spec.dst_addr));
    Chk(_estats_value_copy(&spec->src_port, log->spec.src_port));
    Chk(_estats_value_copy(&spec->src_addr, log->spec.src_addr));

Cleanup:
    return err;
}


estats_error*
estats_log_get_annotation(char** ann, const estats_log* log)
{
    estats_error* err = NULL;

    ErrIf(log->mode != LOG_READ, ESTATS_ERR_INVAL);

    Chk(Strndup(ann, log->note, ANNOTATION_LEN_MAX));

Cleanup:
    return err;
}


estats_error*
estats_log_set_annotation(estats_log* log, const char* ann)
{
    estats_error* err = NULL;

    ErrIf(log->mode != LOG_WRITE, ESTATS_ERR_INVAL);

    Chk(Strndup(&log->note, ann, ANNOTATION_LEN_MAX));

Cleanup:
    return err;
}


estats_error*
estats_log_eof(int* eof, const estats_log* log)
{
    estats_error* err = NULL;

    ErrIf(eof == NULL || log == NULL, ESTATS_ERR_INVAL);

    *eof = feof(log->fp);

Cleanup:
    return err;
}
