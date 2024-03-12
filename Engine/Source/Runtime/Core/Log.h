#pragma once

#include <stdio.h>

/**
 * Logging
 * No Color Right Now :(
 */
class Log {
public:
  static void Init();
  static void Shutdown();

  static void CoreTrace(const char *fmt, ...);
  static void CoreInfo(const char *fmt, ...);
  static void CoreWarn(const char *fmt, ...);
  static void CoreError(const char *fmt, ...);
  static void CoreCritical(const char *fmt, ...);

  static void Trace(const char *fmt, ...);
  static void Info(const char *fmt, ...);
  static void Warn(const char *fmt, ...);
  static void Error(const char *fmt, ...);
  static void Critical(const char *fmt, ...);

private:
  static FILE *core_log_file;
};
