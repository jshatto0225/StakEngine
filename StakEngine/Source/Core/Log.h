#pragma once

// TODO:? For now unicode is not supported
#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Types.h"

class FLog {
public:
    static void Init();

    inline static TRef<spdlog::logger> &GetCoreLogger() { return CoreLogger; }
    inline static TRef<spdlog::logger> &GetClientLogger() { return ClientLogger;  }

private:
    static TRef<spdlog::logger> CoreLogger;
    static TRef<spdlog::logger> ClientLogger;
};

#define SK_LOG_TRACE(...)     ::FLog::GetCoreLogger()->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      ::FLog::GetCoreLogger()->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      ::FLog::GetCoreLogger()->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     ::FLog::GetCoreLogger()->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  ::FLog::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)    ::FLog::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::FLog::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::FLog::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::FLog::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::FLog::GetClientLogger()->critical(__VA_ARGS__)
