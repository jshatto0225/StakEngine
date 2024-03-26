#pragma once

#include <stdio.h>

void LogInit();
void LogShutdown();

void LogCoreTrace(const char *fmt, ...);
void LogCoreInfo(const char *fmt, ...);
void LogCoreWarn(const char *fmt, ...);
void LogCoreError(const char *fmt, ...);
void LogCoreCritical(const char *fmt, ...);

void LogTrace(const char *fmt, ...);
void LogInfo(const char *fmt, ...);
void LogWarn(const char *fmt, ...);
void LogError(const char *fmt, ...);
void LogCritical(const char *fmt, ...);

