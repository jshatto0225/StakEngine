#include "FLog.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

Ref<spdlog::logger> Log::core_logger;
Ref<spdlog::logger> Log::client_logger;

void Log::init() {
	std::vector<spdlog::sink_ptr> LogSinks;
	LogSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	LogSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("StakEngine.log", true));

	LogSinks[0]->set_pattern("%^[%T] %n: %v%$");
	LogSinks[1]->set_pattern("[%T] [%l] %n: %v");

	core_logger = std::make_shared<spdlog::logger>("StakEngine", begin(LogSinks), end(LogSinks));
	spdlog::register_logger(core_logger);
	core_logger->set_level(spdlog::level::trace);
	core_logger->flush_on(spdlog::level::trace);

	client_logger = std::make_shared<spdlog::logger>("Application", begin(LogSinks), end(LogSinks));
	spdlog::register_logger(client_logger);
	client_logger->set_level(spdlog::level::trace);
	client_logger->flush_on(spdlog::level::trace);
}
