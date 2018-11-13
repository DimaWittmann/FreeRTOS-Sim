#include <stdio.h>
#include <stdarg.h>

#include "philosophers.h"

void logMessage(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}


void clearScreen(void)
{
    int res = system("clear");
    if (res) {
        logMessage("system clear failed!");
    }
}
