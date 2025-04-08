#pragma once

// TODO:? For now unicode is not supported
#define FMT_UNICODE 0
#include <spdlog/spdlog.h>

#include "Types.h"

void log_init();

extern std::shared_ptr<spdlog::logger> core_logger;
extern std::shared_ptr<spdlog::logger> client_logger;

#define SK_LOG_TRACE(...)     core_logger->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      core_logger->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      core_logger->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     core_logger->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  core_logger->critical(__VA_ARGS__)

#define LOG_TRACE(...)        client_logger->trace(__VA_ARGS__)
#define LOG_INFO(...)         client_logger->info(__VA_ARGS__)
#define LOG_WARN(...)         client_logger->warn(__VA_ARGS__)
#define LOG_ERROR(...)        client_logger->error(__VA_ARGS__)
#define LOG_CRITICAL(...)     client_logger->critical(__VA_ARGS__)
