#include "Log.h"

Logger::Logger(const std::string& name, bool logFile)
{
    mLogFile = logFile;
    mName = name;
}

void Logger::Info()
{

}

void Logger::Warn()
{

}

void Logger::Error()
{

}

void Logger::Critical()
{

}

Unique<Logger> Log::sCoreLogger;
Unique<Logger> Log::sClientLogger;

void Log::Init()
{
    sCoreLogger = MakeUnique<Logger>("STAK", true);
    sClientLogger = MakeUnique<Logger>("APP", false);
}
