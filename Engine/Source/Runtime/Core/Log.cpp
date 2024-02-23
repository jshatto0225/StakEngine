#include "Log.h"

#include <stdarg.h>

// Goofy aah string concat
static FILE *core_log_file = fopen(SOURCE_DIR "StakRuntime.log", "w");

void sk_debug_core_trace(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [TRACE]: ");
  fprintf(core_log_file, "[StakRuntime] [TRACE]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  vfprintf(core_log_file, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_core_info(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [TRACE]: ");
  fprintf(core_log_file, "[StakRuntime] [INFO]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  vfprintf(core_log_file, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_core_warn(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [TRACE]: ");
  fprintf(core_log_file, "[StakRuntime] [WARN]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  vfprintf(core_log_file, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_core_error(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [TRACE]: ");
  fprintf(core_log_file, "[StakRuntime] [ERROR]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  vfprintf(core_log_file, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_core_critical(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [TRACE]: ");
  fprintf(core_log_file, "[StakRuntime] [CRITICAL]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  vfprintf(core_log_file, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_trace(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [TRACE]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_info(const char *fmt, ...) {
  fprintf(stdout, "[StakRuntime] [INFO]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_warn(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [WARN]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_error(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [ERROR]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(core_log_file, "\n");
}

void sk_debug_critical(const char *fmt, ...) {
  fprintf(stderr, "[StakRuntime] [CRITICAL]: ");
  va_list args;
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  va_end(args);
  fprintf(stderr, "\n");
  fprintf(core_log_file, "\n");
}