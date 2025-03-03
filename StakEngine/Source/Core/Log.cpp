#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

TRef<spdlog::logger> FLog::CoreLogger;
TRef<spdlog::logger> FLog::ClientLogger;

void FLog::Init() {
    std::vector<spdlog::sink_ptr> LogSinks;
    LogSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("StakEngine.log", true));

    LogSinks[0]->set_pattern("%^[%T] %n: %v%$");
    LogSinks[1]->set_pattern("[%T] [%l] %n: %v");

    CoreLogger = std::make_shared<spdlog::logger>("StakEngine", begin(LogSinks), end(LogSinks));
    spdlog::register_logger(CoreLogger);
    CoreLogger->set_level(spdlog::level::trace);
    CoreLogger->flush_on(spdlog::level::trace);

    ClientLogger = std::make_shared<spdlog::logger>("Application", begin(LogSinks), end(LogSinks));
    spdlog::register_logger(ClientLogger);
    ClientLogger->set_level(spdlog::level::trace);
    ClientLogger->flush_on(spdlog::level::trace);
}
