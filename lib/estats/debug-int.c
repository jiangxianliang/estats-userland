#include <estats/estats-int.h>

void
dbgprintf_no_prefix(const char* fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
