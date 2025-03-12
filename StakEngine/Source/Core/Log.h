#pragma once

// TODO:? For now unicode is not supported
#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Types.h"

void LogInit();

extern TRef<spdlog::logger> GCoreLogger;
extern TRef<spdlog::logger> GClientLogger;

#define SK_LOG_TRACE(...)     GCoreLogger->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      GCoreLogger->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      GCoreLogger->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     GCoreLogger->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  GCoreLogger->critical(__VA_ARGS__)

#define LOG_TRACE(...)        GClientLogger->trace(__VA_ARGS__)
#define LOG_INFO(...)         GClientLogger->info(__VA_ARGS__)
#define LOG_WARN(...)         GClientLogger->warn(__VA_ARGS__)
#define LOG_ERROR(...)        GClientLogger->error(__VA_ARGS__)
#define LOG_CRITICAL(...)     GClientLogger->critical(__VA_ARGS__)
