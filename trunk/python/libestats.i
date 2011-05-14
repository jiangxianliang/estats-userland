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

%rename(s_agent) estats_agent;
%rename(s_connection) estats_connection;
%rename(s_snapshot) estats_snapshot;
%rename(s_value) estats_value;

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
        estats_var* s_get_var_head() {
            estats_error* err = NULL;
            estats_var* evar = NULL;
            err = estats_agent_get_var_head(&evar, $self);
            if (err) estats_error_print(stderr, err);
            return evar;
        }
        estats_connection* s_get_connection_head() {
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
        estats_connection* s_next() {
            estats_error* err = NULL;
            estats_connection* econn = NULL;
            err = estats_connection_next(&econn, $self);
            if (err) estats_error_print(stderr, err);
            return econn;
        }
        estats_value* s_read_value(estats_var* _var) {
            estats_error* err = NULL;
            estats_value* _val = NULL;
            err = estats_connection_read_value(&_val, $self, _var);
            if (err) estats_error_print(stderr, err);
            return _val;
        }
};

%extend estats_snapshot {
        estats_snapshot(estats_connection* conn) {
            estats_error* err = NULL;
            estats_snapshot* snap = NULL;
            err = estats_snapshot_alloc(&snap, conn);
            if (err) estats_error_print(stderr, err);
            return snap;
        }
        ~estats_snapshot() {
            estats_snapshot_free(&($self));
        }
        void s_take_snapshot() {
            estats_error* err = NULL;
            err = estats_take_snapshot($self);
            if (err) estats_error_print(stderr, err);
        }
        estats_value* s_read_value(estats_var* _var) {
            estats_error* err = NULL;
            estats_value* _val = NULL;
            err = estats_snapshot_read_value(&_val, $self, _var);
            if (err) estats_error_print(stderr, err);
            return _val;
        }
};

%extend estats_var {
        estats_var* s_next() {
            estats_error* err = NULL;
            estats_var* _var = NULL;
            err = estats_var_next(&_var, $self);
            if (err) estats_error_print(stderr, err);
            return _var;
        }
        const char* s_get_name() {
            estats_error* err = NULL;
            const char* _name;
            err = estats_var_get_name(&_name, $self);
            if (err) estats_error_print(stderr, err);
            return _name;
        }
        ESTATS_TYPE s_get_type() {
            estats_error* err = NULL;
            ESTATS_TYPE _type;
            err = estats_var_get_type(&_type, $self);
            if (err) estats_error_print(stderr, err);
            return _type;
        }
};

%extend estats_value {
        ~estats_value() {
            estats_value_free(&($self));
        }
        char* s_as_string() {
            estats_error* err = NULL;
            char* _strval;
            err = estats_value_as_string(&_strval, $self);
            if (err) estats_error_print(stderr, err);
            return _strval;
        }
};




