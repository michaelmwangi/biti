#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <assert.h>

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


            Logger() = default;
            
            virtual ~Logger() = default;
            virtual void write(const std::string&, LogLevel) = 0;

            Logger(const Logger&) = delete;
            Logger(Logger &&) = delete;
            Logger &operator =(const Logger&) = delete;
            Logger &operator =(Logger&&) = delete;
    };

    class FileLogger: public Logger{
        private:
            std::ofstream log_stream;
            LogLevel log_level;
        public:
            FileLogger(const std::string &, LogLevel) ;
            void write(const std::string&, LogLevel) override;
            
    };

    extern std::shared_ptr<Logger> cur_logger;

    #define LOGGER (Logger::get_current_logger())
}

#endif