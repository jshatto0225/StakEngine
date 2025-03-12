#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

TRef<spdlog::logger> GCoreLogger;
TRef<spdlog::logger> GClientLogger;

void LogInit() {
    std::vector<spdlog::sink_ptr> LogSinks;
    LogSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("StakEngine.log", true));

    LogSinks[0]->set_pattern("%^[%T] %n: %v%$");
    LogSinks[1]->set_pattern("[%T] [%l] %n: %v");

    GCoreLogger = std::make_shared<spdlog::logger>("StakEngine", begin(LogSinks), end(LogSinks));
    spdlog::register_logger(GCoreLogger);
    GCoreLogger->set_level(spdlog::level::trace);
    GCoreLogger->flush_on(spdlog::level::trace);

    GClientLogger = std::make_shared<spdlog::logger>("Application", begin(LogSinks), end(LogSinks));
    spdlog::register_logger(GClientLogger);
    GClientLogger->set_level(spdlog::level::trace);
    GClientLogger->flush_on(spdlog::level::trace);
}
