#pragma once

// TODO:? For now unicode is not supported
#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Types.h"

struct FLog {
    static void Init();

    static TRef<spdlog::logger> CoreLogger;
    static TRef<spdlog::logger> ClientLogger;
};

#define SK_LOG_TRACE(...)     ::FLog::CoreLogger->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      ::FLog::CoreLogger->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      ::FLog::CoreLogger->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     ::FLog::CoreLogger->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  ::FLog::CoreLogger->critical(__VA_ARGS__)

#define LOG_TRACE(...)    ::FLog::ClientLogger->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::FLog::ClientLogger->info(__VA_ARGS__)
#define LOG_WARN(...)     ::FLog::ClientLogger->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::FLog::ClientLogger->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::FLog::ClientLogger->critical(__VA_ARGS__)
