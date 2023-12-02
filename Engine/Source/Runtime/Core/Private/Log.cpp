#include "Log.h"

Logger::Logger(const std::string& name, bool logFile)
{
    mLogFile = logFile;
    mName = name;
}

void Logger::SetLevel(Level level)
{
    // TODO:
}

Unique<Logger> Log::sCoreLogger;
Unique<Logger> Log::sClientLogger;

void Log::Init()
{
    sCoreLogger = MakeUnique<Logger>("STAK", true);
    sClientLogger = MakeUnique<Logger>("APP", false);
}
