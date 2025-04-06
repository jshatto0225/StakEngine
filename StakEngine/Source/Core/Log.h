#pragma once

// TODO:? For now unicode is not supported
#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Types.h"

void log_init();

extern std::shared_ptr<spdlog::logger> global_core_logger;
extern std::shared_ptr<spdlog::logger> g_client_logger;

#define SK_LOG_TRACE(...)     global_core_logger->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      global_core_logger->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      global_core_logger->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     global_core_logger->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  global_core_logger->critical(__VA_ARGS__)

#define LOG_TRACE(...)        g_client_logger->trace(__VA_ARGS__)
#define LOG_INFO(...)         g_client_logger->info(__VA_ARGS__)
#define LOG_WARN(...)         g_client_logger->warn(__VA_ARGS__)
#define LOG_ERROR(...)        g_client_logger->error(__VA_ARGS__)
#define LOG_CRITICAL(...)     g_client_logger->critical(__VA_ARGS__)
