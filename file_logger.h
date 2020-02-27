#pragma once
#include "logger.h"
#include <mutex>
#include <vector>

class FileLogger : public ILogger
{
public:
    ~FileLogger();
    static FileLogger& GetInstance();
    void Log(LogLevel_t logLevel, const char* controlString, ...) override;   

private:
    FileLogger();
    void UpdateLogFile();
    void CreateNewLogFile();
    void WriteToBuffer(const char* buffer);
    void WriteToFile();

    // Member variables
    std::mutex m_fileMutex;
    std::mutex m_logBufferMutex;
    std::string m_activeLogFilename;
    FILE* m_fileStream;
    std::vector<std::string> m_logBuffer;
    time_t m_lastWriteTimestamp;
};

