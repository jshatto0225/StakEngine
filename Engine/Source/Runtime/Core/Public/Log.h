#pragma once

#include <string>

#include "Types.h"

namespace sk {
enum class Level { SKCRITICAL, SKERROR, SKINFO, SKWARN, SKTRACE };

class Logger final {
public:
  Logger(const std::string &name, bool logFile);
  ~Logger();

  // Default Color
  template <typename... Args>
  void trace(const std::string &format, Args &&...args) {
    set_level(Level::SKTRACE);
    std::string message = this->format + this->level + format + "\n";
    printf(message.c_str(), args...);
    fprintf(this->log_file, message.c_str(), args...);
  }
  // Green
  template <typename... Args>
  void info(const std::string &format, Args &&...args) {
    set_level(Level::SKINFO);
    std::string message = this->format + this->level + format + "\n";
    printf(message.c_str(), args...);
    fprintf(this->log_file, message.c_str(), args...);
  }
  // Yellow
  template <typename... Args>
  void warn(const std::string &format, Args &&...args) {
    set_level(Level::SKWARN);
    std::string message = this->format + this->level + format + "\n";
    printf(message.c_str(), args...);
    fprintf(this->log_file, message.c_str(), args...);
  }
  // Bright Red
  template <typename... Args>
  void error(const std::string &format, Args &&...args) {
    set_level(Level::SKERROR);
    std::string message = this->format + this->level + format + "\n";
    printf(message.c_str(), args...);
    fprintf(this->log_file, message.c_str(), args...);
  }
  // Dark Red
  template <typename... Args>
  void critical(const std::string &format, Args &&...args) {
    set_level(Level::SKCRITICAL);
    std::string message = this->format + this->level + format + "\n";
    printf(message.c_str(), args...);
    fprintf(this->log_file, message.c_str(), args...);
  }

private:
  void set_level(Level level);

  std::string name;
  bool log_to_file;
  std::string format;
  std::string level;
  FILE *log_file;
};

class Log final {
public:
  static void init() noexcept;
  static void shutdown() noexcept;
  static Shared<Logger> get_core_logger();
  static Shared<Logger> get_client_logger();

private:
  static Shared<Logger> core_logger;
  static Shared<Logger> client_logger;
  static bool initialized;
};
} // namespace SK

#ifdef SK_DEBUG
#define SK_CORE_TRACE(...) ::sk::Log::get_core_logger()->trace(__VA_ARGS__)
#define SK_CORE_INFO(...) ::sk::Log::get_core_logger()->info(__VA_ARGS__)
#define SK_CORE_WARN(...) ::sk::Log::get_core_logger()->warn(__VA_ARGS__)
#define SK_CORE_ERROR(...) ::sk::Log::get_core_logger()->error(__VA_ARGS__)
#define SK_CORE_CRITICAL(...) ::sk::Log::get_core_logger()->critical(__VA_ARGS__)

#define SK_TRACE(...) ::sk::Log::get_client_logger()->trace(__VA_ARGS__)
#define SK_INFO(...) ::sk::Log::get_client_logger()->info(__VA_ARGS__)
#define SK_WARN(...) ::sk::Log::get_client_logger()->warn(__VA_ARGS__)
#define SK_ERROR(...) ::sk::Log::get_client_logger()->error(__VA_ARGS__)
#define SK_CRITICAL(...) ::sk::Log::get_client_logger()->critical(__VA_ARGS__)
#endif

#ifdef SK_RELEASE
#define SK_CORE_TRACE(...)
#define SK_CORE_INFO(...)
#define SK_CORE_WARN(...)
#define SK_CORE_ERROR(...)
#define SK_CORE_CRITICAL(...)

#define SK_TRACE(...)
#define SK_INFO(...)
#define SK_WARN(...)
#define SK_ERROR(...)
#define SK_CRITICAL(...)
#endif
