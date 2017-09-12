/* Common.h */

#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "String2.h"
#include "Memory.h"

#define PRIVATE static
#define PUBLIC

#define TRACE(x) do { if (DEBUG) dbg_printf x; } while (0)

void dbg_printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}

#endif
