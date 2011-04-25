#include <estats/estats-int.h>

static estats_error* _estats_var_type_to_value_type(ESTATS_VALUE_TYPE* valType, ESTATS_TYPE varType);

estats_error*
_estats_value_from_var_buf(estats_value** value,
                           const void* buf,
                           ESTATS_TYPE type)
{
    estats_error* err = NULL;
    ESTATS_VALUE_TYPE valType;

    ErrIf(value == NULL || buf == NULL, ESTATS_ERR_INVAL);
    *value = NULL;
    
    if (type == ESTATS_TYPE_INET_ADDRESS) {
        type = ((char*) buf)[16] == ESTATS_ADDRTYPE_IPV4 ?
            ESTATS_TYPE_INET_ADDRESS_IPV4 :
            ESTATS_TYPE_INET_ADDRESS_IPV6;
    }

    Chk(_estats_var_type_to_value_type(&valType, type));
    Chk(estats_value_new(value, buf, valType));

 Cleanup:
    if (err != NULL) {
        estats_value_free(value);
    }
    
    return err;
}


estats_error*
_estats_value_to_var_buf(void** buf,
                         size_t* size,
                         const estats_value* value,
                         ESTATS_TYPE type)
{
    estats_error* err = NULL;
    ESTATS_VALUE_TYPE valType;
    estats_value* realValue = NULL;
    void* srcAddr;
    char* b = NULL;
    size_t s;

    ErrIf(buf == NULL || size == NULL || value == NULL, ESTATS_ERR_INVAL);

    /* Convert the value to the required one */
    Chk(_estats_value_copy(&realValue, value));
    Chk(_estats_var_type_to_value_type(&valType, type));
    Chk(estats_value_change_type(&realValue, valType));

    switch (valType) {
    case ESTATS_VALUE_TYPE_UINT16:
        srcAddr = &(realValue->u.u16_val);
        s = 2;
        break;
    case ESTATS_VALUE_TYPE_INT32:
        srcAddr = &(realValue->u.s32_val);
        s = 4;
        break;
    case ESTATS_VALUE_TYPE_UINT32:
        srcAddr = &(realValue->u.u32_val);
        s = 4;
        break;
    case ESTATS_VALUE_TYPE_UINT64:
        srcAddr = &(realValue->u.u64_val);
        s = 8;
        break;
    case ESTATS_VALUE_TYPE_IP4ADDR:
        srcAddr = &(realValue->u.ip4addr_val);
        s = 4;
        break;
    case ESTATS_VALUE_TYPE_IP6ADDR:
        srcAddr = &(realValue->u.ip6addr_val);
        s = 16;
        break;
    case ESTATS_VALUE_TYPE_OCTET:
        srcAddr = &(realValue->u.u8_val);
        s = 1;
        break;
    default:
        Err(ESTATS_ERR_UNHANDLED_VALUE_TYPE);
        break;
    }

    Chk(Malloc((void**) &b, s));
    memcpy(b, srcAddr, s);

    *buf = b;
    b = NULL;
    *size = s;

Cleanup:
    Free((void**) &b);
    estats_value_free(&realValue);

    return err;
}


estats_error*
_estats_value_copy(estats_value** copyVal,
                   const estats_value* origVal)
{
    estats_error* err = NULL;
    estats_value* newVal = NULL;

    ErrIf(copyVal == NULL || origVal == NULL, ESTATS_ERR_INVAL);

    Chk(Malloc((void**) &newVal, sizeof(estats_value)));
    newVal->type = origVal->type;
    switch (origVal->type) {
    case ESTATS_VALUE_TYPE_UINT16:
        newVal->u.u16_val = origVal->u.u16_val;
        break;
    case ESTATS_VALUE_TYPE_INT32:
        newVal->u.s32_val = origVal->u.u64_val;
        break;
    case ESTATS_VALUE_TYPE_UINT32:
        newVal->u.u32_val = origVal->u.u64_val;
        break;
    case ESTATS_VALUE_TYPE_UINT64:
        newVal->u.u64_val = origVal->u.u64_val;
        break;
    case ESTATS_VALUE_TYPE_STRING:
        Chk(Strdup(&(newVal->u.str_val), origVal->u.str_val));
        break;
    case ESTATS_VALUE_TYPE_IP4ADDR:
        newVal->u.ip4addr_val = origVal->u.ip4addr_val;
        break;
    case ESTATS_VALUE_TYPE_IP6ADDR:
        newVal->u.ip6addr_val = origVal->u.ip6addr_val;
        break;
    case ESTATS_VALUE_TYPE_OCTET:
        newVal->u.u8_val = origVal->u.u8_val;
        break;
    default:
        Err(ESTATS_ERR_UNKNOWN_TYPE);
        break;
    }

    *copyVal = newVal;
    newVal = NULL;

Cleanup:
    Free((void**) &newVal);
    return err;
}


estats_error*
estats_value_new(estats_value** value,
                 const void* buf,
                 ESTATS_VALUE_TYPE type)
{
    estats_error* err = NULL;

    ErrIf(value == NULL, ESTATS_ERR_INVAL);
    *value = NULL;

    ErrIf(buf == NULL, ESTATS_ERR_INVAL);
    
    Chk(Malloc((void**) value, sizeof(estats_value)));
    (*value)->type = type;
    (*value)->u.str_val = NULL; /* Just in case */

    switch (type) {
    case ESTATS_VALUE_TYPE_UINT16:
        (*value)->u.u16_val = *((uint16_t*) buf);
        break;
    case ESTATS_VALUE_TYPE_INT32:
        (*value)->u.s32_val = *((int32_t*) buf);
        break;
    case ESTATS_VALUE_TYPE_UINT32:
        (*value)->u.u32_val = *((uint32_t*) buf);
        break;
    case ESTATS_VALUE_TYPE_UINT64:
        (*value)->u.u64_val = *((uint64_t*) buf);
        break;
    case ESTATS_VALUE_TYPE_STRING:
        Chk(Strdup(&((*value)->u.str_val), (char*) buf));
        break;
    case ESTATS_VALUE_TYPE_IP4ADDR:
        (*value)->u.ip4addr_val.s_addr = *((in_addr_t*) buf);
        break;
    case ESTATS_VALUE_TYPE_IP6ADDR:
        memcpy((*value)->u.ip6addr_val.s6_addr, (uint8_t*) buf, 16);
        break;
    case ESTATS_VALUE_TYPE_OCTET:
        (*value)->u.u8_val = *((uint8_t*) buf);
        break;
    default:
        Err(ESTATS_ERR_UNKNOWN_TYPE);
        break;
    }


 Cleanup:
    if (err != NULL) {
        estats_value_free(value);
    }
    
    return err;
}


void
estats_value_free(estats_value** value)
{
    if (value == NULL || *value == NULL)
        return;

    switch ((*value)->type) {
    case ESTATS_VALUE_TYPE_STRING:
        Free((void**) &((*value)->u.str_val));
        break;
    default:
        break;
    }

    Free((void**) value);
}

estats_error*
estats_value_get_type(ESTATS_VALUE_TYPE* type, const estats_value* value)
{
    estats_error* err = NULL;

    ErrIf(value == NULL, ESTATS_ERR_INVAL);

    *type = value->type;

Cleanup:
    return err;
}

estats_error*
estats_value_change_type(estats_value** value,
                         ESTATS_VALUE_TYPE newType)
{
    estats_error* err = NULL;
    ESTATS_VALUE_TYPE oldType;
    estats_value* tmpValue = NULL;
    
    ErrIf(value == NULL || *value == NULL, ESTATS_ERR_INVAL);

    oldType = (*value)->type;
    if (oldType == newType)
        goto Cleanup;

    Chk(Malloc((void**) &tmpValue, sizeof(estats_value)));
    tmpValue->type = newType;
    tmpValue->u.str_val = NULL; /* Just in case */

    if (newType == ESTATS_VALUE_TYPE_STRING) {
        /* Convert to string */
        switch (oldType) {
        case ESTATS_VALUE_TYPE_UINT16:
            Chk(Asprintf(NULL, &(tmpValue->u.str_val), "%u", (*value)->u.u16_val));
            break;
        case ESTATS_VALUE_TYPE_INT32:
            Chk(Asprintf(NULL, &(tmpValue->u.str_val), "%ld", (*value)->u.s32_val));
            break;
        case ESTATS_VALUE_TYPE_UINT32:
            Chk(Asprintf(NULL, &(tmpValue->u.str_val), "%lu", (*value)->u.u32_val));
            break;
        case ESTATS_VALUE_TYPE_UINT64:
            Chk(Asprintf(NULL, &(tmpValue->u.str_val), "%llu", (*value)->u.u64_val));
            break;
        case ESTATS_VALUE_TYPE_IP4ADDR:
            Chk(Malloc((void**) &(tmpValue->u.str_val), INET_ADDRSTRLEN));
            Chk(Inet_ntop(AF_INET, &((*value)->u.ip4addr_val), tmpValue->u.str_val, INET_ADDRSTRLEN));
            break;
        case ESTATS_VALUE_TYPE_IP6ADDR:
            Chk(Malloc((void**) &(tmpValue->u.str_val), INET6_ADDRSTRLEN));
            Chk(Inet_ntop(AF_INET6, &((*value)->u.ip6addr_val), tmpValue->u.str_val, INET6_ADDRSTRLEN));
            break;
        case ESTATS_VALUE_TYPE_OCTET:
            Chk(Asprintf(NULL, &(tmpValue->u.str_val), "%u", (*value)->u.u8_val));
            break;
        default:
            Err(ESTATS_ERR_UNHANDLED_VALUE_TYPE);
            break;
        }

        estats_value_free(value);
        *value = tmpValue;
        tmpValue = NULL;
    } else if (oldType == ESTATS_VALUE_TYPE_STRING) {
        /* Convert from string */
        long int s32;
        unsigned long int u32;
        unsigned long long int u64;
        
        switch (newType) {
        case ESTATS_VALUE_TYPE_UINT16:
            Chk(Strtoul(&u32, (*value)->u.str_val, NULL, 10));
            ErrIf(u32 > 65536, ESTATS_ERR_RANGE);
            tmpValue->u.u16_val = u32;
            break;
        case ESTATS_VALUE_TYPE_INT32:
            Chk(Strtol(&s32, (*value)->u.str_val, NULL, 10));
            tmpValue->u.s32_val = s32;
            break;            
        case ESTATS_VALUE_TYPE_UINT32:
            Chk(Strtoul(&u32, (*value)->u.str_val, NULL, 10));
            tmpValue->u.u32_val = u32;
            break;
        case ESTATS_VALUE_TYPE_UINT64:
            Chk(Strtoull(&u64, (*value)->u.str_val, NULL, 10));
            tmpValue->u.u64_val = u64;
            break;
        case ESTATS_VALUE_TYPE_IP4ADDR:
            Chk(Inet_pton(AF_INET, (*value)->u.str_val, &(tmpValue->u.ip4addr_val)));
            break;
        case ESTATS_VALUE_TYPE_IP6ADDR:
            Chk(Inet_pton(AF_INET6, (*value)->u.str_val, &(tmpValue->u.ip6addr_val)));
            break;
        case ESTATS_VALUE_TYPE_OCTET:
            Chk(Strtoul(&u32, (*value)->u.str_val, NULL, 10));
            ErrIf(u32 > 256, ESTATS_ERR_RANGE);
            tmpValue->u.u8_val = u32;
            break;
        default:
            Err(ESTATS_ERR_UNHANDLED_VALUE_TYPE);
            break;
        }

        estats_value_free(value);
        *value = tmpValue;
        tmpValue = NULL;
    } else {
        /* Convert to string and then from string */
        Chk(estats_value_change_type(value, ESTATS_VALUE_TYPE_STRING));
        Chk(estats_value_change_type(value, newType));
    }
    
 Cleanup:
    estats_value_free(&tmpValue);

    return err;
}


estats_error*
estats_value_as_uint16(uint16_t* u16,
                       const estats_value* value)
{
    estats_error* err = NULL;
    estats_value* newValue = NULL;

    Chk(_estats_value_copy(&newValue, value));
    Chk(estats_value_change_type(&newValue, ESTATS_VALUE_TYPE_UINT16));
    *u16 = newValue->u.u16_val;

 Cleanup:
    estats_value_free(&newValue);
    
    return err;
}


estats_error*
estats_value_as_int32(int32_t* s32,
                      const estats_value* value)
{
    estats_error* err = NULL;
    estats_value* newValue = NULL;

    Chk(_estats_value_copy(&newValue, value));
    Chk(estats_value_change_type(&newValue, ESTATS_VALUE_TYPE_INT32));
    *s32 = newValue->u.s32_val;
    
 Cleanup:
    estats_value_free(&newValue);
    
    return err;
}


estats_error*
estats_value_as_uint32(uint32_t* u32,
                       const estats_value* value)
{
    estats_error* err = NULL;
    estats_value* newValue = NULL;

    Chk(_estats_value_copy(&newValue, value));
    Chk(estats_value_change_type(&newValue, ESTATS_VALUE_TYPE_UINT32));
    *u32 = newValue->u.u32_val;
    
 Cleanup:
    estats_value_free(&newValue);
    
    return err;
}


estats_error*
estats_value_as_uint64(uint64_t* u64,
                       const estats_value* value)
{
    estats_error* err = NULL;
    estats_value* newValue = NULL;

    Chk(_estats_value_copy(&newValue, value));
    Chk(estats_value_change_type(&newValue, ESTATS_VALUE_TYPE_UINT64));
    *u64 = newValue->u.u64_val;
    
 Cleanup:
    estats_value_free(&newValue);
    
    return err;
}


estats_error*
estats_value_as_string(char** str,
                       const estats_value* value)
{
    estats_error* err = NULL;
    estats_value* newValue = NULL;

    Chk(_estats_value_copy(&newValue, value));
    Chk(estats_value_change_type(&newValue, ESTATS_VALUE_TYPE_STRING));
    Chk(Strdup(str, newValue->u.str_val));

 Cleanup:
    estats_value_free(&newValue);
    
    return err;
}


estats_error*
estats_value_as_ip4addr(struct in_addr* ip4addr,
                        const estats_value* value)
{
    estats_error* err = NULL;
    estats_value* newValue = NULL;

    Chk(_estats_value_copy(&newValue, value));
    Chk(estats_value_change_type(&newValue, ESTATS_VALUE_TYPE_IP4ADDR));
    *ip4addr = newValue->u.ip4addr_val;

 Cleanup:
    estats_value_free(&newValue);
    
    return err;
}


estats_error*
estats_value_as_ip6addr(struct in6_addr* ip6addr,
                        const estats_value* value)
{
    estats_error* err = NULL;
    estats_value* newValue = NULL;

    Chk(_estats_value_copy(&newValue, value));
    Chk(estats_value_change_type(&newValue, ESTATS_VALUE_TYPE_IP6ADDR));
    *ip6addr = newValue->u.ip6addr_val;

 Cleanup:
    estats_value_free(&newValue);
    
    return err;
}


estats_error*
estats_value_compare(int* result,
	             const estats_value* v1,
                     const estats_value* v2)
{
    estats_error* err = NULL;
    char* str1 = NULL;
    char* str2 = NULL;

    ErrIf(result == NULL || v1 == NULL || v2 == NULL, ESTATS_ERR_INVAL);
    ErrIf(v1->type != v2->type, ESTATS_ERR_INVAL);

    Chk(estats_value_as_string(&str1, v1));
    Chk(estats_value_as_string(&str2, v2));

    *result = strcmp(str1, str2);

 Cleanup:
    Free((void**) &str1);
    Free((void**) &str2);

    return err;
}


estats_error*
estats_value_difference(estats_value** result,
                        const estats_value* v1,
                        const estats_value* v2)
{
    estats_error* err = NULL;

    ErrIf(result == NULL || v1 == NULL || v2 == NULL, ESTATS_ERR_INVAL);
    ErrIf(v1->type != v2->type, ESTATS_ERR_INVAL);

    switch (v1->type) {
    case ESTATS_VALUE_TYPE_UINT16:
    {
        uint16_t u16 = v1->u.u16_val - v2->u.u16_val;
        Chk(estats_value_new(result, &u16, ESTATS_VALUE_TYPE_UINT16));
        break;
    }
    case ESTATS_VALUE_TYPE_INT32:
    {
        int32_t s32 = v1->u.s32_val - v2->u.s32_val;
        Chk(estats_value_new(result, &s32, ESTATS_VALUE_TYPE_INT32));
        break;
    }
    case ESTATS_VALUE_TYPE_UINT32:
    {
        uint32_t u32 = v1->u.u32_val - v2->u.u32_val;
        Chk(estats_value_new(result, &u32, ESTATS_VALUE_TYPE_UINT32));
        break;
    }
    case ESTATS_VALUE_TYPE_UINT64:
    {
        uint64_t u64 = v1->u.u64_val - v2->u.u64_val;
        Chk(estats_value_new(result, &u64, ESTATS_VALUE_TYPE_UINT64));
        break;
    }
    default:
        Err(ESTATS_ERR_UNHANDLED_VALUE_TYPE);
        break;
    }

 Cleanup:
    return err;
}


static estats_error*
_estats_var_type_to_value_type(ESTATS_VALUE_TYPE* valType,
                               ESTATS_TYPE varType)
{
    estats_error* err = NULL;

    ErrIf(valType == NULL, ESTATS_ERR_INVAL);

    switch (varType) {
    case ESTATS_TYPE_INTEGER:
    case ESTATS_TYPE_INTEGER32:
    case ESTATS_TYPE_COUNTER32:
    case ESTATS_TYPE_GAUGE32:
    case ESTATS_TYPE_UNSIGNED32:
    case ESTATS_TYPE_TIME_TICKS:
        *valType = ESTATS_VALUE_TYPE_UINT32;
        break;
    case ESTATS_TYPE_COUNTER64:
        *valType = ESTATS_VALUE_TYPE_UINT64;
        break;
    case ESTATS_TYPE_INET_PORT_NUMBER: /* Also ..._UNSIGNED16 */
        *valType = ESTATS_VALUE_TYPE_UINT16;
        break;
    case ESTATS_TYPE_INET_ADDRESS_IPV4:
        *valType = ESTATS_VALUE_TYPE_IP4ADDR;
        break;
    case ESTATS_TYPE_INET_ADDRESS_IPV6:
        *valType = ESTATS_VALUE_TYPE_IP6ADDR;
        break;
    case ESTATS_TYPE_OCTET:
        *valType = ESTATS_VALUE_TYPE_OCTET;
        break;
    default:
        Err(ESTATS_ERR_UNKNOWN_TYPE);
        break;
    }

Cleanup:
    return err;
} 
