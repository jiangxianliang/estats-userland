%module libestats
%{
#include <netinet/in.h>
#include <estats/types.h>
#include <estats/types-int.h>
#include <estats/agent.h>
#include <estats/connection.h>
#include <estats/error.h>
#include <estats/snapshot.h>
#include <estats/value.h>
#include <estats/var.h>
%}

%rename(py_agent) estats_agent;
%rename(py_connection) estats_connection;
%rename(py_snapshot) estats_snapshot;

%include <estats/types.h>
%include <estats/types-int.h>
%include <estats/agent.h>
%include <estats/connection.h>
%include <estats/error.h>
%include <estats/snapshot.h>
%include <estats/value.h>
%include <estats/var.h>


%extend estats_agent {
        estats_agent() {
            estats_agent* agent = NULL;
            estats_agent_attach(&agent, ESTATS_AGENT_TYPE_LOCAL, NULL);
            return agent;
        }
        ~estats_agent() {
            estats_agent_detach(&($self));
        }
        estats_var* var_head() {
            estats_error* err = NULL;
            estats_var* evar = NULL;
            err = estats_agent_get_var_head(&evar, $self);
            if (err) estats_error_print(stderr, err);
            return evar;
        }
        estats_connection* connection_head() {
            estats_error* err = NULL;
            estats_connection* econn = NULL;
            err = estats_agent_get_connection_head(&econn, $self);
            if (err) estats_error_print(stderr, err);
            return econn;
        }
        void test_info() {
            printf("feelin' groovy\n");
        }
};

%extend estats_connection {
        estats_connection* connection_next() {
            estats_error* err = NULL;
            estats_connection* econn = NULL;
            err = estats_connection_next(&econn, $self);
            if (err) estats_error_print(stderr, err);
            return econn;
        }
};

%extend estats_snapshot {
        estats_snapshot(estats_connection* conn) {
            estats_error* err = NULL;
            estats_snapshot* snap = NULL;
            estats_snapshot_alloc(&snap, conn);
            return snap;
        }
        ~estats_snapshot() {
            estats_snapshot_free(&($self));
        }
};

%extend estats_var {
        estats_var* var_next() {
            estats_error* err = NULL;
            estats_var* _var = NULL;
            err = estats_var_next(&_var, $self);
            if (err) estats_error_print(stderr, err);
            return _var;
        }
        const char* return_name() {
            estats_error* err = NULL;
            const char* _name;
            err = estats_var_get_name(&_name, $self);
            if (err) estats_error_print(stderr, err);
            return _name;
        }
        ESTATS_TYPE return_type() {
            estats_error* err = NULL;
            ESTATS_TYPE _type;
            err = estats_var_get_type(&_type, $self);
            if (err) estats_error_print(stderr, err);
            return _type;
        }
};

