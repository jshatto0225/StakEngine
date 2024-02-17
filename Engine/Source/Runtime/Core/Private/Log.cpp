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

static Shared<Logger> core_logger;
static Shared<Logger> client_logger;

static bool initialized = false;

void log_init() noexcept {
  core_logger = new_shared<Logger>("StakEngine", true);
  client_logger = new_shared<Logger>("Application", false);
  initialized = true;
}

void log_shutdown() noexcept {
  if (initialized) {
    core_logger.reset();
    client_logger.reset();
    initialized = false;
  }
}

Shared<Logger> log_get_core_logger() {
  if (!initialized) {
    fprintf(stderr, "Log Not Initialized");
  }
  return core_logger;
}

Shared<Logger> log_get_client_logger() {
  if (!initialized) {
    fprintf(stderr, "Log Not Initialized");
  }
  return client_logger;
}
} // namespace sk
