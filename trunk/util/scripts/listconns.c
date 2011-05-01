#include "scripts.h"

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_conninfo* ci_head = NULL;
    estats_conninfo* ci_pos;

    printf("%-8s %-8s %-8s %-10s %-20s %-8s %-20s %-8s\n", "CID", "PID", "UID", "Cmdline", "SrcAddr", "SrcPort", "DstAddr", "DstPort");
    printf("-------- -------- -------- ---------- -------------------- -------- -------------------- --------\n");

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));

    Chk(estats_get_conninfo_head(&ci_head, agent));

    ESTATS_CONNINFO_FOREACH(ci_pos, ci_head) {
        int cid, pid, uid;
        char cmdline[20];
        struct spec_ascii spec_asc;

        cid = ci_pos->cid;
        pid = ci_pos->pid;
        uid = ci_pos->uid;
        
        strncpy(cmdline, ci_pos->cmdline, 10);
        cmdline[10] = '\0';

        Chk(estats_connection_spec_as_strings(&spec_asc, &ci_pos->spec));

        printf("%-8d %-8d %-8d %-10s %-20s %-8s %-20s %-8s\n", cid, pid, uid, cmdline, spec_asc.src_addr, spec_asc.src_port, spec_asc.dst_addr, spec_asc.dst_port);

        free(spec_asc.dst_addr);
        free(spec_asc.dst_port);
        free(spec_asc.src_addr);
        free(spec_asc.src_port);
    }

Cleanup:
    estats_conninfo_free(&ci_head);
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
