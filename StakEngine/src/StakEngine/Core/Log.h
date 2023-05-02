#pragma once

#include "Base.h"

#include <spdlog/spdlog.h>

namespace Stak
{
	class STAK_API Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#ifdef STK_DEBUG
	#define STK_CORE_TRACE(...)		::Stak::Log::GetCoreLogger()->trace(__VA_ARGS__);
	#define STK_CORE_INFO(...)		::Stak::Log::GetCoreLogger()->info(__VA_ARGS__);
	#define STK_CORE_WARN(...)		::Stak::Log::GetCoreLogger()->warn(__VA_ARGS__);
	#define STK_CORE_ERROR(...)		::Stak::Log::GetCoreLogger()->error(__VA_ARGS__);
	#define STK_CORE_CRITICAL(...)	::Stak::Log::GetCoreLogger()->critical(__VA_ARGS__);

	#define STK_TRACE(...)			::Stak::Log::GetClientLogger()->trace(__VA_ARGS__);
	#define STK_INFO(...)			::Stak::Log::GetClientLogger()->info(__VA_ARGS__);
	#define STK_WARN(...)			::Stak::Log::GetClientLogger()->warn(__VA_ARGS__);
	#define STK_ERROR(...)			::Stak::Log::GetClientLogger()->error(__VA_ARGS__);
	#define STK_CRITICAL(...)		::Stak::Log::GetClientLogger()->critical(__VA_ARGS__);
#else
	#define STK_CORE_TRACE(...)
	#define STK_CORE_INFO(...)
	#define STK_CORE_WARN(...)
	#define STK_CORE_ERROR(...)
	#define STK_CORE_FATAL(...)

	#define STK_TRACE(...)
	#define STK_INFO(...)
	#define STK_WARN(...)
	#define STK_ERROR(...)
	#define STK_FATAL(...)
#endif