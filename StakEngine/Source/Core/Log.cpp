#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

std::shared_ptr<spdlog::logger> global_core_logger;
std::shared_ptr<spdlog::logger> g_client_logger;

void log_init() {
    std::vector<spdlog::sink_ptr> log_sinks;
    log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("StakEngine.log", true));

    log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
    log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

    global_core_logger = std::make_shared<spdlog::logger>("StakEngine", begin(log_sinks), end(log_sinks));
    spdlog::register_logger(global_core_logger);
    global_core_logger->set_level(spdlog::level::trace);
    global_core_logger->flush_on(spdlog::level::trace);

    g_client_logger = std::make_shared<spdlog::logger>("Application", begin(log_sinks), end(log_sinks));
    spdlog::register_logger(g_client_logger);
    g_client_logger->set_level(spdlog::level::trace);
    g_client_logger->flush_on(spdlog::level::trace);
}
