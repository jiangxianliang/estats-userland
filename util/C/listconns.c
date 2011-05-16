#include "scripts.h"

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_sockinfo* sinfo = NULL;
    estats_sockinfo_item* ci_head = NULL;
    estats_sockinfo_item* ci_pos;

    printf("%-8s %-8s %-8s %-10s %-20s %-8s %-20s %-8s\n", "CID", "PID", "UID", "Cmdline", "SrcAddr", "SrcPort", "DstAddr", "DstPort");
    printf("-------- -------- -------- ---------- -------------------- -------- -------------------- --------\n");

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));

    Chk(estats_sockinfo_new(&sinfo, agent));

    Chk(estats_sockinfo_get_list_head(&ci_head, sinfo));

    ESTATS_SOCKINFO_FOREACH(ci_pos, ci_head) {
        int cid, pid, uid;
        char* cmdline;
        struct estats_connection_spec spec;
        struct spec_ascii spec_asc;

        estats_sockinfo_get_cid(&cid, ci_pos);
        estats_sockinfo_get_pid(&pid, ci_pos);
        estats_sockinfo_get_uid(&uid, ci_pos);

        Chk(estats_sockinfo_get_cmdline(&cmdline, ci_pos));
        Chk(estats_sockinfo_get_connection_spec(&spec, ci_pos));
        Chk(estats_connection_spec_as_strings(&spec_asc, &spec));

        printf("%-8d %-8d %-8d %-10s %-20s %-8s %-20s %-8s\n", cid, pid, uid, cmdline, spec_asc.src_addr, spec_asc.src_port, spec_asc.dst_addr, spec_asc.dst_port);

        free((void*) cmdline);
    }

Cleanup:
    estats_sockinfo_free(&sinfo);
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
