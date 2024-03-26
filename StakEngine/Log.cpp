#include "Log.h"

#include <stdarg.h>

FILE *CoreLogFile;

void LogInit() {
  fopen_s(&CoreLogFile, "StakRuntime.log", "w");
}

void LogShutdown() {
  fclose(CoreLogFile);
}

void LogCoreTrace(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [TRACE]: ");
  fprintf(CoreLogFile, "[StakRuntime] [TRACE]: ");
  va_list args1;
  va_list args2;
  va_start(args1, fmt);
  va_start(args2, fmt);
  vfprintf(stdout, fmt, args1);
  vfprintf(CoreLogFile, fmt, args2);
  va_end(args1);
  va_end(args2);
  fprintf(stdout, "\n");
  fprintf(CoreLogFile, "\n");
}

void LogCoreInfo(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [INFO]: ");
  fprintf(CoreLogFile, "[StakRuntime] [TRACE]: ");
  va_list args1;
  va_list args2;
  va_start(args1, fmt);
  va_start(args2, fmt);
  vfprintf(stdout, fmt, args1);
  vfprintf(CoreLogFile, fmt, args2);
  va_end(args1);
  va_end(args2);
  fprintf(stdout, "\n");
  fprintf(CoreLogFile, "\n");
}

void LogCoreWarn(const char *fmt, ...) {
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

void LogCoreError(const char *fmt, ...) {
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

void LogCoreCritical(const char *fmt, ...) {
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

void LogTrace(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [TRACE]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
}

void LogInfo(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [INFO]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
}

void LogWarn(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [WARN]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

void LogError(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [ERROR]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

void LogCritical(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [CRITICAL]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}
