#include "FLog.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

TRef<spdlog::logger> FLog::sCoreLogger;
TRef<spdlog::logger> FLog::sClientLogger;

void FLog::Init() {
	std::vector<spdlog::sink_ptr> LogSinks;
	LogSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("StakEngine.log", true));

	LogSinks[0]->set_pattern("%^[%T] %n: %v%$");
	LogSinks[1]->set_pattern("[%T] [%l] %n: %v");

	sCoreLogger = std::make_shared<spdlog::logger>("StakEngine", begin(LogSinks), end(LogSinks));
	spdlog::register_logger(sCoreLogger);
	sCoreLogger->set_level(spdlog::level::trace);
	sCoreLogger->flush_on(spdlog::level::trace);

	sClientLogger = std::make_shared<spdlog::logger>("Application", begin(LogSinks), end(LogSinks));
	spdlog::register_logger(sClientLogger);
	sClientLogger->set_level(spdlog::level::trace);
	sClientLogger->flush_on(spdlog::level::trace);
}
