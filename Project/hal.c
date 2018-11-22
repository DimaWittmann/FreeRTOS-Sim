#include <stdarg.h>


void logMessage(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}


void busy_wait(void)
{
    volatile unsigned int i;
    for (i = 0; i < 100000000UL; i++) ;
    (void) i;
}
