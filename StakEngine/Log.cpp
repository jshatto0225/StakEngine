#include "Log.h"

#include <stdarg.h>

#include "Platform.h"

/*********************
 * Private Interface *
 *********************/

static FILE *CoreLogFile;

/********************
 * Public Interface *
 ********************/

void
LogInit()
{
    CoreLogFile = fopen(ROOT_DIR "StakRuntime.log", "w");
}

void
LogShutdown()
{
    fclose(CoreLogFile);
}

void
LogCoreTrace(const char *fmt, ...)
{
    fprintf(stderr, "[StakRuntime] [TRACE]: ");
    fprintf(CoreLogFile, "[StakRuntime] [TRACE]: ");
    va_list args1;
    va_list args2;
    va_start(args1, fmt);
    va_start(args2, fmt);
    vfprintf(stderr, fmt, args1);
    vfprintf(CoreLogFile, fmt, args2);
    va_end(args1);
    va_end(args2);
    fprintf(stderr, "\n");
    fprintf(CoreLogFile, "\n");
}

void
LogCoreInfo(const char *fmt, ...)
{
    fprintf(stderr, "[StakRuntime] [INFO]: ");
    fprintf(CoreLogFile, "[StakRuntime] [TRACE]: ");
    va_list args1;
    va_list args2;
    va_start(args1, fmt);
    va_start(args2, fmt);
    vfprintf(stderr, fmt, args1);
    vfprintf(CoreLogFile, fmt, args2);
    va_end(args1);
    va_end(args2);
    fprintf(stderr, "\n");
    fprintf(CoreLogFile, "\n");
}

void
LogCoreWarn(const char *fmt, ...)
{
    fprintf(stderr, "[StakRuntime] [WARN]: ");
    fprintf(CoreLogFile, "[StakRuntime] [WARN]: ");
    va_list args1;
    va_list args2;
    va_start(args1, fmt);
    va_start(args2, fmt);
    vfprintf(stderr, fmt, args1);
    vfprintf(CoreLogFile, fmt, args2);
    va_end(args1);
    va_end(args2);
    fprintf(stderr, "\n");
    fprintf(CoreLogFile, "\n");
}

void
LogCoreError(const char *fmt, ...)
{
    fprintf(stderr, "[StakRuntime] [ERROR]: ");
    fprintf(CoreLogFile, "[StakRuntime] [ERROR]: ");
    va_list args1;
    va_list args2;
    va_start(args1, fmt);
    va_start(args2, fmt);
    vfprintf(stderr, fmt, args1);
    vfprintf(CoreLogFile, fmt, args2);
    va_end(args1);
    va_end(args2);
    fprintf(stderr, "\n");
    fprintf(CoreLogFile, "\n");
}

void
LogCoreCritical(const char *fmt, ...)
{
    fprintf(stderr, "[StakRuntime] [CRITICAL]: ");
    fprintf(CoreLogFile, "[StakRuntime] [CRITICAL]: ");
    va_list args1;
    va_list args2;
    va_start(args1, fmt);
    va_start(args2, fmt);
    vfprintf(stderr, fmt, args1);
    vfprintf(CoreLogFile, fmt, args2);
    va_end(args1);
    va_end(args2);
    fprintf(stderr, "\n");
    fprintf(CoreLogFile, "\n");
}

void
LogTrace(const char *fmt, ...)
{
    fprintf(stderr, "[App] [TRACE]: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void
LogInfo(const char *fmt, ...)
{
    fprintf(stderr, "[App] [INFO]: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void
LogWarn(const char *fmt, ...)
{
    fprintf(stderr, "[App] [WARN]: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void
LogError(const char *fmt, ...)
{
    fprintf(stderr, "[App] [ERROR]: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

void
LogCritical(const char *fmt, ...)
{
    fprintf(stderr, "[App] [CRITICAL]: ");
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}
