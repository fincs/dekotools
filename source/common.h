#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <getopt.h>
#ifdef _WIN32
#include <fcntl.h>
#endif
#include "types.h"
#include "FileClass.h"

#if !defined(_WIN32) && !defined(stricmp)
#define stricmp strcasecmp
#endif
