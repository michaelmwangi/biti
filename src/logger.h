#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>
#include <string.h>
#include <iostream>
#include <memory>
#include <ctime>
#include <cstdarg>
#include <chrono>
#include <cstring>
#include <unordered_map>

#include "consts.h"


namespace biti {
    
    enum class LogLevel{
        NONE,
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        SEVERE
    };
    
    class Logger {
        public:
            static std::shared_ptr<Logger> get_current_logger();

            static void create_file_logger(const std::string &);

            std::string error_no_msg(){
                // return a string version of the current errno
                return (strerror(errno));
            }
            
            Logger() = default;
            
            virtual ~Logger() = default;
            virtual void log(LogLevel, const char *, ...) = 0;
            std::string get_log_level(LogLevel);
            
            Logger(const Logger&) = delete;
            Logger(Logger &&) = delete;
            Logger &operator =(const Logger&) = delete;
            Logger &operator =(Logger&&) = delete;
    };

    class FileLogger: public Logger{
        private:
            std::fstream log_stream;
            LogLevel log_level;
        public:
            FileLogger(const std::string &, LogLevel) ;
            void log(LogLevel, const char *, ...) override;
            
    };

    extern std::shared_ptr<Logger> cur_logger;

    #define LOGGER (Logger::get_current_logger())
}

#endif