#pragma once

#include <spdlog/spdlog.h>

#include "Types.h"

class Log final {
public:
    static void init();

    inline static Ref<spdlog::logger> &get_core_logger() { return core_logger; }
    inline static Ref<spdlog::logger> &get_client_logger() { return client_logger;  }
    
private:
    static Ref<spdlog::logger> core_logger;
    static Ref<spdlog::logger> client_logger;
};

#define SK_LOG_TRACE(...)     ::Log::get_core_logger()->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      ::Log::get_core_logger()->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      ::Log::get_core_logger()->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     ::Log::get_core_logger()->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  ::Log::get_core_logger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)    ::Log::get_client_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Log::get_client_logger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Log::get_client_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Log::get_client_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Log::get_client_logger()->critical(__VA_ARGS__)
