#include "log.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include "utils/utils.hpp"

static std::shared_ptr<spdlog::logger> core_logger;
static std::shared_ptr<spdlog::logger> client_logger;

void Log::init() {
	Array<spdlog::sink_ptr> log_sinks;
	log_sinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
	log_sinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("stak_engine.log", true));

	log_sinks[0]->set_pattern("%^[%T] %n: %v%$");
	log_sinks[1]->set_pattern("[%T] [%l] %n: %v");

	core_logger = std::make_shared<spdlog::logger>("stak_engine", begin(log_sinks), end(log_sinks));
	spdlog::register_logger(core_logger);
	core_logger->set_level(spdlog::level::trace);
	core_logger->flush_on(spdlog::level::trace);

	client_logger = std::make_shared<spdlog::logger>("app", begin(log_sinks), end(log_sinks));
	spdlog::register_logger(client_logger);
	client_logger->set_level(spdlog::level::trace);
	client_logger->flush_on(spdlog::level::trace);
}


std::shared_ptr<spdlog::logger> Log::get_core_logger() {
	return core_logger;
}

std::shared_ptr<spdlog::logger> Log::get_client_logger() {
	return client_logger;
}
