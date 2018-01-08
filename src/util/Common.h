/* Common.h */

#ifndef _COMMON_
#define _COMMON_

#include <stdio.h>
#include <stdlib.h>

#define PRIVATE static
#define PUBLIC

#include "Object.h"
#include "String2.h"
#include "Memory.h"
#include "Debug.h"

#define TRACE(x) do { if (DEBUG) dbg_printf x; } while (0)

#endif
