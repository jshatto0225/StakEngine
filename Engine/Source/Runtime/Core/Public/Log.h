#pragma once

#include <string>

#include "Types.h"

namespace SK
{
    enum class Level
    {
        SKCRITICAL,
        SKERROR,
        SKINFO,
        SKWARN,
        SKTRACE
    };

    class Logger final
    {
    public:
        Logger(const std::string& name, bool logFile);
        ~Logger();

        // Default Color
        template<typename ...Args>
        void Trace(const std::string& format, Args&& ...args)
        {
            SetLevel(Level::SKTRACE);
            std::string message = mFormat + mLevel + format + "\n";
            printf(message.c_str(), args...);
            fprintf(mLogFile, message.c_str(), args...);
        }
        // Green
        template<typename ...Args>
        void Info(const std::string& format, Args&& ...args)
        {
            SetLevel(Level::SKINFO);
            std::string message = mFormat + mLevel + format + "\n";
            printf(message.c_str(), args...);
            fprintf(mLogFile, message.c_str(), args...);
        }
        // Yellow
        template<typename ...Args>
        void Warn(const std::string& format, Args&& ...args)
        {
            SetLevel(Level::SKWARN);
            std::string message = mFormat + mLevel + format + "\n";
            printf(message.c_str(), args...);
            fprintf(mLogFile, message.c_str(), args...);
        }
        // Bright Red
        template<typename ...Args>
        void Error(const std::string& format, Args&& ...args)
        {
            SetLevel(Level::SKERROR);
            std::string message = mFormat + mLevel + format + "\n";
            printf(message.c_str(), args...);
            fprintf(mLogFile, message.c_str(), args...);
        }
        // Dark Red
        template<typename ...Args>
        void Critical(const std::string& format, Args&& ...args)
        {
            SetLevel(Level::SKCRITICAL);
            std::string message = mFormat + mLevel + format + "\n";
            printf(message.c_str(), args...);
            fprintf(mLogFile, message.c_str(), args...);
        }

    private:
        void SetLevel(Level level);

        std::string mName;
        bool mLogToFile;
        std::string mFormat;
        std::string mLevel;
        FILE* mLogFile;
    };

    class Log final
    {
    public:
        static void Init() noexcept;
        static void Shutdown() noexcept;
        static Shared<Logger> GetCoreLogger();
        static Shared<Logger> GetClientLogger();

    private:
        static Shared<Logger> sCoreLogger;
        static Shared<Logger> sClientLogger;
        static bool sInitialized;
    };
}

#ifdef SK_DEBUG
#define SK_CORE_TRACE(...)      ::SK::Log::GetCoreLogger()->Trace(__VA_ARGS__)
#define SK_CORE_INFO(...)       ::SK::Log::GetCoreLogger()->Info(__VA_ARGS__)
#define SK_CORE_WARN(...)       ::SK::Log::GetCoreLogger()->Warn(__VA_ARGS__)
#define SK_CORE_ERROR(...)      ::SK::Log::GetCoreLogger()->Error(__VA_ARGS__)
#define SK_CORE_CRITICAL(...)   ::SK::Log::GetCoreLogger()->Critical(__VA_ARGS__)

#define SK_TRACE(...)           ::SK::Log::GetClientLogger()->Trace(__VA_ARGS__)
#define SK_INFO(...)            ::SK::Log::GetClientLogger()->Info(__VA_ARGS__)
#define SK_WARN(...)            ::SK::Log::GetClientLogger()->Warn(__VA_ARGS__)
#define SK_ERROR(...)           ::SK::Log::GetClientLogger()->Error(__VA_ARGS__)
#define SK_CRITICAL(...)        ::SK::Log::GetClientLogger()->Critical(__VA_ARGS__)
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

