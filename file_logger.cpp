#include <assert.h>
#include "file_logger.h"
#include <stdarg.h>
#include <sstream>
#include <iostream>
#include <Windows.h>

//-----------------------------------------------------------------------------------------------------------------
// Constants
//-----------------------------------------------------------------------------------------------------------------
// if blank, then current location would be used
const char* BASE_LOG_LOCATION = "C:\\Users\\m0pxnn\\Source\\Repos\\LogFileManager\\LogFiles";  

// Max size of buffer after which data from it will be written to file
const size_t MAX_LOG_BUFFER_SIZE = 500;        

// Duration after which data from buffer will be written to file
const int LOG_FILE_WRITE_INTERVAL_SECONDS = 600; 

// Maximum allowed size of log file
const long MAX_LOG_FILE_SIZE_BYTES = 5 * 1024;// *1024;


//-----------------------------------------------------------------------------------------------------------------
// @name                    : GetFileSize
//
// @description             : Fetches size (in bytes) of the specified filename
//-----------------------------------------------------------------------------------------------------------------
static long GetFileSize(const std::string & filename)
{
    struct stat stat_buf;
    int rc = stat(filename.c_str(), &stat_buf);
    return rc == 0 ? stat_buf.st_size : -1;
}


//-----------------------------------------------------------------------------------------------------------------
// @name                    : GetFilename
//
// @description             : Prepares a filename based on current timestamp.
//-----------------------------------------------------------------------------------------------------------------
static std::string GetFilename()
{
    std::stringstream ss;
    ss << time(nullptr);
    std::string timestamp = ss.str();

    std::string baseLocation(BASE_LOG_LOCATION);
    std::string filename;
    if (baseLocation[baseLocation.size() - 1] == '\\')
    {
        filename = baseLocation + "logfile_" + timestamp + ".log";
    }
    else
    {
        filename = baseLocation + "\\logfile_" + timestamp + ".log";
    }

    return filename;
}

//------------------------------------------------------------------------------------------
// Class member functions
//------------------------------------------------------------------------------------------
FileLogger::FileLogger()
{
    m_fileStream = nullptr;
    m_activeLogFilename = "";
    m_lastWriteTimestamp = time(nullptr);
}


FileLogger::~FileLogger()
{
}

//-----------------------------------------------------------------------------------------------------------------
// GetInstance
//-----------------------------------------------------------------------------------------------------------------
FileLogger& FileLogger::GetInstance()
{
    static FileLogger fileLogger;
    return fileLogger;
}

//-----------------------------------------------------------------------------------------------------------------
// @name                    : Log
//
// @description             : Logs the provided string. Logging is initially done in buffer. Once the buffer 
//                            capacity is reached (MAX_LOG_BUFFER_SIZE) or last write duration (LOG_FILE_WRITE_INTERVAL_SECONDS)
//                            is exceeded then the buffer content is dumped to the log file.
//-----------------------------------------------------------------------------------------------------------------
void FileLogger::Log(LogLevel_t logLevel, const char* controlString, ...)
{
    char commonInfo[COMMON_INFO_MAX_LEN]{};
    PrepareCommonInfo(logLevel, commonInfo);

    va_list ap;
    va_start(ap, controlString);
  
    char logInfo[LOG_INFO_MAX_LEN]{};
    vsprintf_s(logInfo, controlString, ap);
    va_end(ap);

    // Write common log related info to file
    char buffer[COMMON_INFO_MAX_LEN + LOG_INFO_MAX_LEN + 1]{};
    sprintf_s(buffer, "%s %s", commonInfo, logInfo);

    // Write actual log message to buffer
    WriteToBuffer(buffer);
}

//-----------------------------------------------------------------------------------------------------------------
// @name                    : WriteToBuffer
//
// @description             : Writes the given content to log buffer.
//-----------------------------------------------------------------------------------------------------------------
void FileLogger::WriteToBuffer(const char *buffer)
{
    std::string log(buffer);   

    m_logBufferMutex.lock();    
    m_logBuffer.push_back(log);
    size_t logBufferSize = m_logBuffer.size();
    m_logBufferMutex.unlock();
    
    // If buffer size is reached or last dump duration has exceeded, dump the logs into the logfile
    if (logBufferSize >= MAX_LOG_BUFFER_SIZE || (time(nullptr) - m_lastWriteTimestamp) > LOG_FILE_WRITE_INTERVAL_SECONDS)
    {
        UpdateLogFile();
        WriteToFile();
    }
}

//-----------------------------------------------------------------------------------------------------------------
// @name                    : WriteToFile
//
// @description             : Dumps the content of log buffer to the log file.
//-----------------------------------------------------------------------------------------------------------------
void FileLogger::WriteToFile()
{
    // Open the active log file
    std::lock_guard<std::mutex> lockFile(m_fileMutex);
    fopen_s(&m_fileStream, m_activeLogFilename.c_str(), "a");
    std::cout << "Writing " << m_logBuffer.size() << " logs to " << m_activeLogFilename << std::endl;

    assert(m_fileStream);

    std::lock_guard<std::mutex> lockBuffer(m_logBufferMutex);   
    for (auto it = m_logBuffer.begin(); it != m_logBuffer.end(); it++)
    {
        std::string data = *it;
        fprintf(m_fileStream, "%s", data.c_str());
    }

    // Close the file
    fclose(m_fileStream);
    m_fileStream = nullptr;
    std::cout << "File closed" << std::endl;

    // Clear the buffer
    m_logBuffer.clear();

    m_lastWriteTimestamp = time(nullptr);
}

//-----------------------------------------------------------------------------------------------------------------
// @name                    : UpdateLogFile
//
// @description             : Updates the m_activeLogFilename.
//-----------------------------------------------------------------------------------------------------------------
void FileLogger::UpdateLogFile()
{
    std::lock_guard<std::mutex> lock(m_fileMutex);
    if (m_activeLogFilename.empty())
    {
        // Create a new log file
        std::cout << "No active log file. Create new." << std::endl;
        CreateNewLogFile();
    }
    else if (GetFileSize(m_activeLogFilename) >= MAX_LOG_FILE_SIZE_BYTES)
    {
        std::cout << "Log file size exceeded. Create new." << std::endl;

        std::string prevLogFile = m_activeLogFilename;        
        m_activeLogFilename.clear();
        CreateNewLogFile();

        // This makes sure that we can track chain of log files (if multiple logs were created)
        fopen_s(&m_fileStream, m_activeLogFilename.c_str(), "a");
        if (m_fileStream)
        {
            std::string newFileMsg = "********** PREVIOUS LOG FILE: " + prevLogFile + " **********\n\n";
            fputs(newFileMsg.c_str(), m_fileStream);
            fclose(m_fileStream);
            m_fileStream = nullptr;
        }
    }
    else
    {
        // DO NOTHING
        // Use existing m_activeLogFilename for writing
        std::cout << "Using existing log file for writing" << std::endl;
    }
}

//-----------------------------------------------------------------------------------------------------------------
// @name                    : CreateNewLogFile
//
// @description             : Updates the m_activeLogFilename as per the current timestamp.
//-----------------------------------------------------------------------------------------------------------------
void FileLogger::CreateNewLogFile()
{   
    if (m_activeLogFilename.empty())
    {
        // Fetch new log file name
        m_activeLogFilename = GetFilename();
    }
}