#pragma once

#include <stdio.h>

namespace Log {

void Init();

void Shutdown();

namespace Core {

void Trace(const char *fmt, ...);

void Info(const char *fmt, ...);

void Warn(const char *fmt, ...);

void Error(const char *fmt, ...);

void Critical(const char *fmt, ...);

} // namespace Core

void Trace(const char *fmt, ...);

void Info(const char *fmt, ...);

void Warn(const char *fmt, ...);

void Error(const char *fmt, ...);

void Critical(const char *fmt, ...);

} // namespace Log
