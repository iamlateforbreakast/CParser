/* Common.h */

#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>
#include <stdlib.h>

#include "String2.h"
#include "Memory.h"

#define PRIVATE static
#define PUBLIC

#define TRACE(x) do { if (DEBUG) dbg_printf x; } while (0)

#endif
