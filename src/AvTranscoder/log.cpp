#include "log.hpp"

namespace avtranscoder
{

std::string Logger::logHeaderMessage = "";

void callbackToWriteInFile(void* ptr, int level, const char* fmt, va_list vl)
{
    std::ofstream outputFile;
    outputFile.open(LOG_FILE, std::ios::out | std::ios::app);

#ifdef AVTRANSCODER_FFMPEG_DEPENDENCY
    // Format a line of log the same way as the default callback
    char line[1024];
    static int print_prefix = 1;
    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix); // only available with ffmpeg
    outputFile << line;
#else
    outputFile << "Warning: currently can't log in file with avtranscoder when depending on libav" << std::endl;
#endif

    outputFile.close();
}

void Logger::setLogLevel(const int level)
{
    av_log_set_level(level);
}

void Logger::log(const int level, const std::string& msg)
{
    // add header message
    std::string levelStr;
    switch(level)
    {
        case AV_LOG_DEBUG:
            levelStr = "debug";
            break;
        case AV_LOG_INFO:
            levelStr = "info";
            break;
        case AV_LOG_WARNING:
            levelStr = "warning";
            break;
        case AV_LOG_ERROR:
            levelStr = "error";
            break;
        default:
            break;
    }
    std::string logMessage = "[avTranscoder - " + levelStr + "] ";

    // add content message
    logMessage += msg;
    logMessage += "\n";

    // send message
    av_log(NULL, level, "%s", logMessage.c_str());
}

void Logger::logInFile()
{
    av_log_set_callback(callbackToWriteInFile);

    // clean log file
    std::ofstream outputFile;
    outputFile.open(LOG_FILE);
    outputFile.close();
}
}
