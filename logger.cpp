#include "logger.h"
#include <thread>

// Disable _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

ILogger::ILogger()
{

}

ILogger::~ILogger()
{

}

//-----------------------------------------------------------------------------------------------------------------
// @name                    : PrepareCommonInfo
//
// @description             : Creates a string with the log level and current timestamp. This info is present
//                            in all the log entries.
//-----------------------------------------------------------------------------------------------------------------
void ILogger::PrepareCommonInfo(LogLevel_t logLevel, char * commonInfo)
{
    time_t ts = time(0);
    char* localDateTime = ctime(&ts);   
    char* newline = strstr(localDateTime, "\n");
    if (newline)
    {
        // ctime() returns a string ending in '\n'. Remove that
        *newline = '\0';
    }

    const char* debugLevel = g_logLevelStringMap.at(logLevel);   
    sprintf(commonInfo, "%s [%s]", localDateTime, debugLevel);
}