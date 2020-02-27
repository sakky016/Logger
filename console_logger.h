#pragma once
#include "logger.h"
#include <mutex>

class ConsoleLogger : public ILogger
{
public:
    ~ConsoleLogger();
    static ConsoleLogger& GetInstance();
    void Log(LogLevel_t logLevel, const char* controlString, ...) override;

private:
    ConsoleLogger();  // ConsoleLogger object can be created only using GetInstance()
    std::mutex m_outstreamMutex;
};

