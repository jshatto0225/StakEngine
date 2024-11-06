#pragma once

#include <spdlog/spdlog.h>

#include "Types.h"

namespace Stak {

class Log {
public:
  static void init();

  inline static Ref<spdlog::logger> &getCoreLogger() { return s_CoreLogger; }
  inline static Ref<spdlog::logger> &getClientLogger() { return s_ClientLogger;  }

private:
  static Ref<spdlog::logger> s_CoreLogger;
  static Ref<spdlog::logger> s_ClientLogger;
};

} // namespace Stak

#define SK_LOG_TRACE(...)     ::Stak::Log::getCoreLogger()->trace(__VA_ARGS__)
#define SK_LOG_INFO(...)      ::Stak::Log::getCoreLogger()->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      ::Stak::Log::getCoreLogger()->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     ::Stak::Log::getCoreLogger()->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  ::Stak::Log::getCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)    ::Stak::Log::getClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Stak::Log::getClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Stak::Log::getClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Stak::Log::getClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Stak::Log::getClientLogger()->critical(__VA_ARGS__)
