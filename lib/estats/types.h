#if !defined(ESTATS_TYPES_H)
#define ESTATS_TYPES_H

#if !defined(NULL)
# define NULL 0
#endif

#define ESTATS_FOREACH_FLAGS_IS_FIRST (1 << 0)
#define ESTATS_FOREACH_FLAGS_IS_LAST  (1 << 1)

typedef enum ESTATS_ADDRTYPE {
    ESTATS_ADDRTYPE_UNKNOWN = 0,
    ESTATS_ADDRTYPE_IPV4,
    ESTATS_ADDRTYPE_IPV6,
    ESTATS_ADDRTYPE_DNS = 16
} ESTATS_ADDRTYPE;

typedef enum ESTATS_AGENT_TYPE {
    ESTATS_AGENT_TYPE_LOCAL = 0,
    ESTATS_AGENT_TYPE_LOG = 1,
} ESTATS_AGENT_TYPE;

typedef enum ESTATS_ERROR {
    ESTATS_ERR_SUCCESS              = 0,
    ESTATS_ERR_FILE                 = 1,
    ESTATS_ERR_AGENT_TYPE           = 2,
    ESTATS_ERR_NOMEM                = 3,
    ESTATS_ERR_NOCONNECTION         = 4,
    ESTATS_ERR_INVAL                = 5,
    ESTATS_ERR_HEADER               = 6,
    ESTATS_ERR_NOVAR                = 7,
    ESTATS_ERR_NOGROUP              = 8,
    ESTATS_ERR_SOCK                 = 9,
    ESTATS_ERR_KERNVER              = 10,
    ESTATS_ERR_NOIMPL               = 11,
    ESTATS_ERR_UNKNOWN              = 12,
    ESTATS_ERR_LIBC                 = 13,
    ESTATS_ERR_GROUP                = 14,
    ESTATS_ERR_CUSTOM               = 15,
    ESTATS_ERR_EOF                  = 16,
    ESTATS_ERR_UNKNOWN_TYPE         = 17,
    ESTATS_ERR_UNHANDLED_VALUE_TYPE = 18,
    ESTATS_ERR_RANGE                = 19,
    ESTATS_ERR_STRING_CONVERSION    = 20,
    ESTATS_ERR_ACCESS               = 21,
    ESTATS_ERR_CHKSUM               = 22,
} ESTATS_ERROR;

typedef enum ESTATS_FOREACH_RET {
    ESTATS_FOREACH_STOP = 0,
    ESTATS_FOREACH_CONTINUE = 1,
} ESTATS_FOREACH_RET;

typedef enum ESTATS_LOG_MODE {
    ESTATS_LOG_READ = 0,
    ESTATS_LOG_WRITE = 1,
} ESTATS_LOG_MODE;

typedef enum ESTATS_TYPE {
    ESTATS_TYPE_INTEGER = 0,
    ESTATS_TYPE_INTEGER32 = 1,
    ESTATS_TYPE_INET_ADDRESS_IPV4 = 2,
    ESTATS_TYPE_IP_ADDRESS = ESTATS_TYPE_INET_ADDRESS_IPV4, /* Deprecated */
    ESTATS_TYPE_COUNTER32 = 3,
    ESTATS_TYPE_GAUGE32 = 4,
    ESTATS_TYPE_UNSIGNED32 = 5,
    ESTATS_TYPE_TIME_TICKS = 6,
    ESTATS_TYPE_COUNTER64 = 7,
    ESTATS_TYPE_INET_PORT_NUMBER = 8,
    ESTATS_TYPE_UNSIGNED16 = ESTATS_TYPE_INET_PORT_NUMBER, /* Deprecated */
    ESTATS_TYPE_INET_ADDRESS = 9,
    ESTATS_TYPE_INET_ADDRESS_IPV6 = 10,
} ESTATS_TYPE;

typedef enum ESTATS_VALUE_TYPE {
    ESTATS_VALUE_TYPE_UINT16  = 0,
    ESTATS_VALUE_TYPE_INT32   = 1,
    ESTATS_VALUE_TYPE_UINT32  = 2,
    ESTATS_VALUE_TYPE_UINT64  = 3,
    ESTATS_VALUE_TYPE_STRING  = 4,
    ESTATS_VALUE_TYPE_IP4ADDR = 5,
    ESTATS_VALUE_TYPE_IP6ADDR = 6,
} ESTATS_VALUE_TYPE;

struct estats_connection_spec {
    struct estats_value *dst_port;
    struct estats_value *dst_addr;
    struct estats_value *src_port;
    struct estats_value *src_addr;
};

typedef struct estats_agent      estats_agent;
typedef struct estats_connection estats_connection;
typedef struct estats_error      estats_error;
typedef struct estats_group      estats_group;
typedef struct estats_log        estats_log;
typedef struct estats_snapshot   estats_snapshot;
typedef struct estats_value      estats_value;
typedef struct estats_var        estats_var;

typedef ESTATS_FOREACH_RET (*estats_connection_foreach_func)(estats_connection* conn, int flags, void* userData);
typedef ESTATS_FOREACH_RET (*estats_group_foreach_func)(estats_group* group, int flags, void* userData);
typedef ESTATS_FOREACH_RET (*estats_var_foreach_func)(estats_var* _var, int flags, void* _userData);

#endif /* !defined(ESTATS_TYPES_H) */
