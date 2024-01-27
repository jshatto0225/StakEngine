#include "Log.h"

#include <iostream>

Logger::Logger(const std::string& name, bool logToFile)
{
    mLogToFile = logToFile;
    mName = name;
    mFormat = "[" + name + "] ";
    SetLevel(Level::SKTRACE);
    if(logToFile)
    {
        mLogFile = fopen((name + ".log").c_str(), "w");
    }
}

Logger::~Logger()
{
    if(mLogToFile)
    {
        fclose(mLogFile);
    }
}

void Logger::SetLevel(Level level)
{
    // TODO: Switch to using colors
    
    switch(level)
    {
        case Level::SKCRITICAL:
        {
            mLevel = "[CRITICAL]: ";
        } break;
        case Level::SKERROR:
        {
            mLevel = "[ERROR]: ";
        } break;
        case Level::SKINFO:
        {
            mLevel = "[INFO]: ";
        } break;
        case Level::SKTRACE:
        {
            mLevel = "[TRACE]: ";
        } break;
        case Level::SKWARN:
        {
            mLevel = "[WARN]: ";
        } break;
    }
}

Shared<Logger> Log::sCoreLogger;
Shared<Logger> Log::sClientLogger;

bool Log::sInitialized = false;

void Log::Init() noexcept
{
    sCoreLogger = MakeShared<Logger>("StakEngine", true);
    sClientLogger = MakeShared<Logger>("Application", false);
    sInitialized = true;
}

void Log::Shutdown() noexcept
{
    if(!sInitialized)
    {
        SK_CORE_WARN("Log already shutdown");
    }
    else
    {
        sCoreLogger.reset();
        sClientLogger.reset();
        sInitialized = false;
    }
}

Shared<Logger> Log::GetCoreLogger()
{
    if(!sInitialized)
    {
        throw std::runtime_error("Core Logger Is NULL");
    }
    else
    {
        return sCoreLogger;
    }
}

Shared<Logger> Log::GetClientLogger()
{
    if(!sInitialized)
    {
        throw std::runtime_error("Client Logger Is NULL");
    }
    else
    {
        return sClientLogger;
    }
}
