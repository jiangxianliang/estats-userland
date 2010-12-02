#include <estats/estats-int.h>

int
strisspace(const char* str)
{
    int c;

    while ((c = *str++) != '\0') {
        if (!isspace(c))
            return 0;
    }

    return 1;
}
