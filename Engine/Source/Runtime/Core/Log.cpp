#include "Log.h"

#include <stdarg.h>

FILE *Log::core_log_file;

void Log::Init() {
  // Goofy aah string concat
  Log::core_log_file = fopen(SOURCE_DIR "StakRuntime.log", "w");
}

void Log::Shutdown() {
  fclose(Log::core_log_file);
}

void Log::CoreTrace(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [TRACE]: ");
  fprintf(Log::core_log_file, "[StakRuntime] [TRACE]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  vfprintf(Log::core_log_file, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
  fprintf(Log::core_log_file, "\n");
}

void Log::CoreInfo(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [INFO]: ");
  fprintf(Log::core_log_file, "[StakRuntime] [TRACE]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  vfprintf(Log::core_log_file, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
  fprintf(Log::core_log_file, "\n");
}

void Log::CoreWarn(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [WARN]: ");
  fprintf(Log::core_log_file, "[StakRuntime] [WARN]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  vfprintf(Log::core_log_file, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(Log::core_log_file, "\n");
}

void Log::CoreError(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [ERROR]: ");
  fprintf(Log::core_log_file, "[StakRuntime] [ERROR]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  vfprintf(Log::core_log_file, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(Log::core_log_file, "\n");
}

void Log::CoreCritical(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [CRITICAL]: ");
  fprintf(Log::core_log_file, "[StakRuntime] [CRITICAL]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  vfprintf(Log::core_log_file, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(Log::core_log_file, "\n");
}

void Log::Trace(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [TRACE]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
}

void Log::Info(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [INFO]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
}

void Log::Warn(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [WARN]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

void Log::Error(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [ERROR]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}

void Log::Critical(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [CRITICAL]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
}
