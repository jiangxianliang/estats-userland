#include "scripts.h"

int
main(int argc, char *argv[])
{
    estats_error* err = NULL;
    estats_agent* agent = NULL;
    estats_connection* conn;

    printf("%-12s %-40s %-8s %-40s %-8s\n", "CID", "SrcAddr", "SrcPort", "DstAddr", "DstPort");
    printf("------------ ---------------------------------------- -------- ---------------------------------------- --------\n");

    Chk(estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL));
    Chk(estats_agent_get_connection_head(&conn, agent));
    while (conn != NULL) {
        int cid;
        struct estats_connection_spec spec;
	char* srcAddr = NULL;
       	char* dstAddr = NULL;
       	char* srcPort = NULL;
       	char* dstPort = NULL;

        Chk(estats_connection_get_cid(&cid, conn));
	Chk(estats_connection_get_spec(&spec, conn)); 

	Chk(estats_value_as_string(&srcAddr, spec.src_addr));
       	Chk(estats_value_as_string(&srcPort, spec.src_port));
       	Chk(estats_value_as_string(&dstAddr, spec.dst_addr));
       	Chk(estats_value_as_string(&dstPort, spec.dst_port));

        printf("%-12d %-40s %-8s %-40s %-8s\n", cid, srcAddr, srcPort, dstAddr, dstPort);

	free(srcAddr);
       	free(dstAddr);
       	free(srcPort);
       	free(dstPort);
        Chk(estats_connection_next(&conn, conn));
    }
    
 Cleanup:
    estats_agent_detach(&agent);

    if (err != NULL) {
        PRINT_AND_FREE(err);
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
