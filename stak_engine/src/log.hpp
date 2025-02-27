#pragma once

#include <spdlog/spdlog.h>

void init_log();

std::shared_ptr<spdlog::logger> get_core_logger();
std::shared_ptr<spdlog::logger> get_client_logger();

#define SK_LOG_TRACE(...)     get_core_logger()->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      get_core_logger()->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      get_core_logger()->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     get_core_logger()->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  get_core_logger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)    get_client_logger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     get_client_logger()->info(__VA_ARGS__)
#define LOG_WARN(...)     get_client_logger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    get_client_logger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) get_client_logger()->critical(__VA_ARGS__)
