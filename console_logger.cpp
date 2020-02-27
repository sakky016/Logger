#include "console_logger.h"
#include <iostream>
#include <stdarg.h>

ConsoleLogger::ConsoleLogger() : ILogger()
{
}


ConsoleLogger::~ConsoleLogger()
{
}

ConsoleLogger& ConsoleLogger::GetInstance()
{
    static ConsoleLogger consoleLogger;
    return consoleLogger;
}

//-----------------------------------------------------------------------------------------------------------------
// @name                    : Log
//
// @description             : Logs the provided string to console
//-----------------------------------------------------------------------------------------------------------------
void ConsoleLogger::Log(LogLevel_t logLevel, const char* controlString, ...)
{
    // Make this logging thread safe
    const std::lock_guard<std::mutex> lock(m_outstreamMutex);

    va_list ap;
    va_start(ap, controlString);

    char commonInfo[COMMON_INFO_MAX_LEN]{};
    PrepareCommonInfo(logLevel, commonInfo);
    
    // Write common log related info to console
    printf("%s ", commonInfo);

    // Write actual log message to console
    vprintf(controlString, ap);
    va_end(ap);
}