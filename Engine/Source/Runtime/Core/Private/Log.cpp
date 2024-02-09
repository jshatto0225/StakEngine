#include "Log.h"

namespace sk {
Logger::Logger(const std::string &name, bool log_to_file) {
  this->log_to_file = log_to_file;
  this->name = name;
  this->format = "[" + name + "] ";
  set_level(Level::SKTRACE);
  if (log_to_file) {
    this->log_file = fopen((name + ".log").c_str(), "w");
  }
}

Logger::~Logger() {
  if (log_to_file) {
    fclose(log_file);
  }
}

void Logger::set_level(Level level) {
  // TODO: Switch to using colors

  switch (level) {
  case Level::SKCRITICAL: {
    this->level = "[CRITICAL]: ";
  } break;
  case Level::SKERROR: {
    this->level = "[ERROR]: ";
  } break;
  case Level::SKINFO: {
    this->level = "[INFO]: ";
  } break;
  case Level::SKTRACE: {
    this->level = "[TRACE]: ";
  } break;
  case Level::SKWARN: {
    this->level = "[WARN]: ";
  } break;
  }
}

Shared<Logger> Log::core_logger;
Shared<Logger> Log::client_logger;

bool Log::initialized = false;

void Log::init() noexcept {
  core_logger = new_shared<Logger>("StakEngine", true);
  client_logger = new_shared<Logger>("Application", false);
  initialized = true;
}

void Log::shutdown() noexcept {
  if (initialized) {
    core_logger.reset();
    client_logger.reset();
    initialized = false;
  }
}

Shared<Logger> Log::get_core_logger() {
  if (!initialized) {
    throw std::runtime_error("Core Logger Is NULL");
  } else {
    return core_logger;
  }
}

Shared<Logger> Log::get_client_logger() {
  if (!initialized) {
    throw std::runtime_error("Client Logger Is NULL");
  } else {
    return client_logger;
  }
}
} // namespace SK
