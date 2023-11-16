#pragma once

#include <vector>
#include <string>

class Logger
{
public:
    Logger(const std::string& name, bool logFile);

    // Default Color
    template<typename ... Args>
    void Trace(const std::string& format, Args&& ... args)
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
    std::string name;
    bool logFile;
};

class Log
{
public:
    static void Init();
    static Logger* CoreLogger;
    static Logger* ClientLogger;
};

#ifdef SK_DEBUG
#define SK_CORE_TRACE(...)      Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_INFO(...)       Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_WARN(...)       Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_ERROR(...)      Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_CORE_CRITICAL(...)   Log::CoreLogger->Trace(__VA_ARGS__)

#define SK_TRACE(...)           Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_INFO(...)            Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_WARN(...)            Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_ERROR(...)           Log::CoreLogger->Trace(__VA_ARGS__)
#define SK_CRITICAL(...)        Log::CoreLogger->Trace(__VA_ARGS__)
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

