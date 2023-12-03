#include "Log.h"

#include <iostream>

Logger::Logger(const std::string& name, bool logToFile)
{
    mLogToFile = logToFile;
    mName = name;
    mFormat = "[" + name + "] ";
    SetLevel(Level::SK_TRACE);
    if(logToFile)
    {
        mLogFile = fopen((name + ".log").c_str(), "w");
    }
}

Logger::~Logger()
{
    fclose(mLogFile);
}

void Logger::SetLevel(Level level)
{
    // TODO: Switch to using colors

    switch(level)
    {
    case Level::SK_CRITICAL:
    {
        mLevel = "[CRITICAL]: ";
    } break;
    case Level::SK_ERROR:
    {
        mLevel = "[ERROR]: ";
    } break;
    case Level::SK_INFO:
    {
        mLevel = "[INFO]: ";
    } break;
    case Level::SK_TRACE:
    {
        mLevel = "[TRACE]: ";
    } break;
    case Level::SK_WARN:
    {
        mLevel = "[WARN]: ";
    } break;
    }
}

Unique<Logger> Log::sCoreLogger;
Unique<Logger> Log::sClientLogger;

void Log::Init()
{
    sCoreLogger = MakeUnique<Logger>("StakEngine", true);
    sClientLogger = MakeUnique<Logger>("Application", false);
}

void Log::Shutdown()
{
    sCoreLogger = nullptr;
    sClientLogger = nullptr;
}
