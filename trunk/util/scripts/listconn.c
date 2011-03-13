#include "scripts.h"

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_conninfo* ci_head = NULL;
    estats_conninfo* ci_pos;

    printf("%-12s %-20s %-40s %-8s %-40s %-8s\n", "CID", "Cmdline", "SrcAddr", "SrcPort", "DstAddr", "DstPort");
    printf("------------ -------------------- ---------------------------------------- -------- ---------------------------------------- --------\n");

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
/*
    Chk(estats_get_conninfo_head(&ci_head, agent));

    ESTATS_CONNINFO_FOREACH(ci_pos, ci_head) {
        int cid;
        char cmdline[20];
	char* srcAddr = NULL;
       	char* dstAddr = NULL;
       	char* srcPort = NULL;
       	char* dstPort = NULL;

        cid = ci_pos->cid;
        strncpy(&cmdline, ci_pos->cmdline, 20);
        cmdline[19] = '\0';

	Chk(estats_value_as_string(&srcAddr, (ci_pos->spec).src_addr));
       	Chk(estats_value_as_string(&srcPort, (ci_pos->spec).src_port));
       	Chk(estats_value_as_string(&dstAddr, (ci_pos->spec).dst_addr));
       	Chk(estats_value_as_string(&dstPort, (ci_pos->spec).dst_port));

        printf("%-12d %-20s %-40s %-8s %-40s %-8s\n", cid, cmdline, srcAddr, srcPort, dstAddr, dstPort);

        free(srcAddr);
        free(dstAddr);
        free(srcPort);
        free(dstPort);
    }
*/
Cleanup:
    estats_conninfo_free(&ci_head);
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
