#include "console_logger.h"
#include "file_logger.h"
#include <iostream>
#include <thread>
#include <Windows.h>

const int MAX_WAIT_DURATION_MS = 100;
const int LOG_MAX_LEN = 500;


//-----------------------------------------------------------------------------------------------------------------
// @name                    : LogTester
//
// @description             : A dummy thread function which continuously logs data.
//-----------------------------------------------------------------------------------------------------------------
void LogTester()
{
    // Provide a new seed for this thread   
    auto id = std::hash<std::thread::id>{}(std::this_thread::get_id());    
    srand(time(nullptr) + id);

    while (1)
    {
        int level = rand() % LOG_MAX_LEVELS;
        int waitDuration = rand() % MAX_WAIT_DURATION_MS;
        size_t msgLen = rand() % LOG_MAX_LEN;
        std::string msg;
        for (auto i = 0; i < msgLen; i++)
        {
            char randomChar = rand() % 123;
            if (randomChar >= 48 && randomChar < 123)
            {
                msg.push_back(randomChar);
            }
        }

        msg.push_back('\n');

        //std::cout << msg << std::endl;
        FileLogger::GetInstance().Log(static_cast<LogLevel_t>(level), msg.c_str());
        Sleep(waitDuration);
    } // Keep writing logs
}

//---------------------------------------------------------------------------
// M A I N
//---------------------------------------------------------------------------
int main()
{
    std::thread loggerThread1(LogTester);
    std::thread loggerThread2(LogTester);
    std::thread loggerThread3(LogTester);
    std::thread loggerThread4(LogTester);
    loggerThread1.join();
    loggerThread2.join();
    loggerThread3.join();
    loggerThread4.join();

    return 0;
}
