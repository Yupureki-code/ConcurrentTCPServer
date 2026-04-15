#include "../include/logstrategy.h"

#include <cstdio>
#include <cstdlib>

namespace ns_log
{
    static void log_exit(const char* message)
    {
        perror(message);
        exit(EXIT_FAILURE);
    }

    std::string LogLevelToString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARNING";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
        }
    }

    std::string get_current_time()
    {
        time_t tm = time(nullptr);
        struct tm curr;
        localtime_r(&tm, &curr);
        char timebuffer[64];
        snprintf(timebuffer, sizeof(timebuffer), "%4d-%02d-%02d %02d:%02d:%02d",
                 curr.tm_year + 1900,
                 curr.tm_mon + 1,
                 curr.tm_mday,
                 curr.tm_hour,
                 curr.tm_min,
                 curr.tm_sec);
        return timebuffer;
    }

    void ConsoleLogStrategy::synclog(const std::string message)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        std::cout << message << std::endl;
    }

    FileLogStrategy::FileLogStrategy(const std::string path, const std::string name)
        : _path(path), _name(name)
    {
        _file = _path + (_path.back() == '/' ? "" : "/") + _name;
        std::fstream file;
        file.open(_file.c_str(), std::ios::in | std::ios::out);
        if (!file.is_open())
        {
            file.open(_file.c_str(), std::ios::out);
            file.close();
        }
    }

    void FileLogStrategy::synclog(const std::string message)
    {
        std::lock_guard<std::mutex> guard(_mutex);
        std::ofstream fout(_file.c_str(), std::ios::app);
        if (!fout.is_open())
        {
            log_exit("fout");
            return;
        }
        fout << message << "\n";
    }

    Logger::Logger()
        : _strategy(std::make_unique<ConsoleLogStrategy>())
    {
        _enabled_levels[LogLevel::DEBUG] = true;
        _enabled_levels[LogLevel::INFO] = true;
        _enabled_levels[LogLevel::WARNING] = true;
        _enabled_levels[LogLevel::ERROR] = true;
        _enabled_levels[LogLevel::FATAL] = true;
    }

    void Logger::enable_console_log_strategy()
    {
        _strategy = std::make_unique<ConsoleLogStrategy>();
    }

    void Logger::enable_file_log_strategy(const std::string path, const std::string name)
    {
        _strategy = std::make_unique<FileLogStrategy>(path, name);
    }

    Logger& Logger::GetInstance()
    {
        return Logger::_inst;
    }
    void Logger::EnableLogLevel(LogLevel level)
    {
        _enabled_levels[level] = true;
    }
    void Logger::DisableLogLevel(LogLevel level)
    {
        _enabled_levels[level] = false;
    }
    bool Logger::IsLogLevelEnabled(LogLevel level)
    {        
        return _enabled_levels[level];
    }
    Logger::LogMessage::LogMessage(const LogLevel type, const std::string filename, const int line, Logger& logger)
        : _type(type), _curr_time(get_current_time()), _pid(getpid()), _filename(filename), _line(line), _logger(logger)
    {
        if(!logger.IsLogLevelEnabled(type))
            return;
        std::stringstream ssbuffer;
        ssbuffer << "[" << _curr_time << "]"
                << "[" << LogLevelToString(type) << "]"
                << "[" << _pid << "]"
                << "[" << _filename << "]"
                << "[" << _line << "]"
                << " - ";
        _loginfo = ssbuffer.str();
    }

    Logger::LogMessage::~LogMessage()
    {
        _logger._strategy->synclog(_loginfo);
    }

    Logger::LogMessage Logger::operator()(const LogLevel level,const int line,const std::string filename)
    {
        return LogMessage(level,filename,line,*this);
    }

    Logger Logger::_inst;
}