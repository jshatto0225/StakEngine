#include "Log.h"

Logger::Logger(const std::string& name, bool logFile)
{
    this->logFile = logFile;
    this->name = name;
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

Logger* Log::CoreLogger;
Logger* Log::ClientLogger;

void Log::Init()
{
    CoreLogger = new Logger("STAK", true);
    ClientLogger = new Logger("APP", false);
}
