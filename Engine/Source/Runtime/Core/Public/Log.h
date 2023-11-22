#pragma once

#include <vector>
#include <string>

#include "Types.h"

class Logger final
{
public:
    Logger(const std::string& name, bool logFile);

    // Default Color
    template<typename ...Args>
    void Trace(const std::string& format, Args&& ...args)
    {
        printf((format + "\n").c_str(), args...);
    }
    // Green
    void Info();
    // Yellow
    void Warn();
    // Bright Red
    void Error();
    // Dark Red
    void Critical();

private:
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
#define SK_CORE_INFO(...)       Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_WARN(...)       Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_ERROR(...)      Log::sCoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_CRITICAL(...)   Log::sCoreLogger->Trace(__VA_ARGS__)

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

