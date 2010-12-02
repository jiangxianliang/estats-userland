/************************************************************
 * File:      logtest.c					    *
 * Author(s): John Estebrook, NCSA, jestabro@ncsa.uiuc.edu  *
 *            Raghu Reddy, PSC,     rreddy@psc.edu	    *
 * Date:      6/28/2002                                     *
 ************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "estats/estats.h"
//
// Note: Error checking not coded for brevity
//
int main(int argc, char **argv)
{
    estats_agent      *agent;
    estats_group      *group;
    estats_connection *conn;
    estats_snapshot   *snap;
    estats_log 	      *log;
    estats_var 	      *var = NULL;

    char buf[8];
    int ii;

    if(argc<2) {
	printf("Usage: logtest cid\n");
	exit(EXIT_FAILURE);
    }

    agent = estats_attach(ESTATS_AGENT_TYPE_LOCAL, NULL);
    group = estats_group_find(agent, "read");
    conn  = estats_connection_lookup(agent, atoi(argv[1]));
    snap  = estats_snapshot_alloc(group, conn); 

    log = estats_log_open_write("logtest.txt", conn, group);

    for(ii=0;ii<3;ii++) {
	estats_snap(snap);
	estats_log_write(log, snap);
	printf("writing snapshot %d\n", ii);
	sleep(1);
    }

    estats_log_close_write(log);
    estats_snapshot_free(snap); 
    snap = NULL;
    log = NULL;
    //
    // Now post process; Normally a different run
    //
    log   = estats_log_open_read("logtest.txt");

    agent = estats_get_log_agent(log);
    group = estats_get_log_group(log);
    conn  = estats_get_log_connection(log);
    snap  = estats_snapshot_alloc(group, conn);

    var   = estats_var_find(group, "CurrTime");

    for(ii=0;ii<3;ii++) { 
       	estats_snap_from_log(snap, log); 
       	estats_snap_read(var, snap, &buf);
       	printf("CurrTime is: %s\n", estats_value_to_text(ESTATS_TYPE_COUNTER32, &buf));
    }

    estats_log_close_read(log);

    return;
}
