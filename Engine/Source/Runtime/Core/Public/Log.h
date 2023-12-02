#pragma once

#include <vector>
#include <string>

#include "Types.h"

#define CRITICAL_STRING "Color 0C"
#define ERROR_STRING "Color 0A"
#define INFO_STRING "Color 02"
#define WARN_STIRNG "Color 06"
#define TRACE_STRING "Color 08"

enum class Level
{
    SK_CRITICAL,
    SK_ERROR,
    SK_INFO,
    SK_WARN,
    SK_TRACE
};

class Logger final
{
public:
    Logger(const std::string& name, bool logFile);

    // Default Color
    template<typename ...Args>
    void Trace(const std::string& format, Args&& ...args)
    {
        SetLevel(Level::SK_TRACE);
        printf((format + "\n").c_str(), args...);
    }
    // Green
    template<typename ...Args>
    void Info(const std::string& format, Args&& ...args)
    {
        SetLevel(Level::SK_INFO);
        printf((format + "\n").c_str(), args...);
    }
    // Yellow
    template<typename ...Args>
    void Warn(const std::string& format, Args&& ...args)
    {
        SetLevel(Level::SK_WARN);
        printf((format + "\n").c_str(), args...);
    }
    // Bright Red
    template<typename ...Args>
    void Error(const std::string& format, Args&& ...args)
    {
        SetLevel(Level::SK_ERROR);
        printf((format + "\n").c_str(), args...);
    }
    // Dark Red
    template<typename ...Args>
    void Critical(const std::string& format, Args&& ...args)
    {
        SetLevel(Level::SK_CRITICAL);
        printf((format + "\n").c_str(), args...);
    }

private:
    static void SetLevel(Level level);

    std::string mName;
    bool mLogFile;
};

class Log final
{
public:
    static void Init();
    static Unique<Logger> sCoreLogger;
    static Unique<Logger> sClientLogger;
};

#ifdef SK_DEBUG
#define SK_CORE_TRACE(...)      Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_INFO(...)       Log::sCoreLogger->Info(__VA_ARGS__)
#define SK_CORE_WARN(...)       Log::sCoreLogger->Warn(__VA_ARGS__)
#define SK_CORE_ERROR(...)      Log::sCoreLogger->Error(__VA_ARGS__)
#define SK_CORE_CRITICAL(...)   Log::sCoreLogger->Critical(__VA_ARGS__)

#define SK_TRACE(...)           Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_INFO(...)            Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_WARN(...)            Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_ERROR(...)           Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_CRITICAL(...)        Log::sCoreLogger->Trace(__VA_ARGS__)
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

