#pragma once

#include <spdlog/spdlog.h>

namespace Log {
void init();

std::shared_ptr<spdlog::logger> get_core_logger();
std::shared_ptr<spdlog::logger> get_client_logger();
    
}

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
