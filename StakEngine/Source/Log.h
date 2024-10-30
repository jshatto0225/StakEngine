#pragma once

#include <spdlog/spdlog.h>

#include "Types.h"

namespace Stak {

class Log {
public:
  static void Init();

  inline static Ref<spdlog::logger> &GetCoreLogger() { return s_CoreLogger; }
  inline static Ref<spdlog::logger> &GetClientLogger() { return s_ClientLogger;  }

private:
  static Ref<spdlog::logger> s_CoreLogger;
  static Ref<spdlog::logger> s_ClientLogger;
};

} // namespace Stak

#define SK_LOG_TRACE(...)     ::Stak::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SK_CORE_INFO(...)     ::Stak::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SK_LOG_WARN(...)      ::Stak::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SK_LOG_ERROR(...)     ::Stak::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SK_LOG_CRITICAL(...)  ::Stak::Log::GetCoreLogger()->critical(__VA_ARGS__)

#define LOG_TRACE(...)    ::Stak::Log::GetClientLogger()->trace(__VA_ARGS__)
#define LOG_INFO(...)     ::Stak::Log::GetClientLogger()->info(__VA_ARGS__)
#define LOG_WARN(...)     ::Stak::Log::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_ERROR(...)    ::Stak::Log::GetClientLogger()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::Stak::Log::GetClientLogger()->critical(__VA_ARGS__)
