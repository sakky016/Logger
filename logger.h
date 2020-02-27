#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <map>

const size_t COMMON_INFO_MAX_LEN = 100;
const size_t LOG_INFO_MAX_LEN = 500;

typedef enum LogLevel_tag
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL,
    LOG_MAX_LEVELS // This should always be last
}LogLevel_t;

const std::map<LogLevel_t, const char*> g_logLevelStringMap = { {LOG_DEBUG, "DEBUG"},
                                                                {LOG_INFO, "INFO"},
                                                                {LOG_WARNING, "WARNING"},
                                                                {LOG_ERROR, "ERROR"},
                                                                {LOG_CRITICAL, "CRITICAL"} };


class ILogger
{
public:
    ILogger();
    ~ILogger();
    void PrepareCommonInfo(LogLevel_t logLevel, char *commonInfo);
    virtual void Log(LogLevel_t logLevel, const char* controlString, ...) = 0;
};


#endif
