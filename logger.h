#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>

namespace biti {
    
    class Logger {
        public:
            static std::shared_ptr<Logger> get_current_logger();

            static void create_file_logger(const std::string &);

            Logger() = default;
            
            virtual ~Logger() = default;
            virtual std::ostream &get_stream() = 0;

            Logger(const Logger&) = delete;
            Logger(Logger &&) = delete;
            Logger &operator =(const Logger&) = delete;
            Logger &operator =(Logger&&) = delete;
    };

    class FileLogger: public Logger{
        private:
            std::ofstream log_stream;
        public:
            FileLogger(const std::string &) ;
            std::ostream &get_stream() override;
    };

    static std::shared_ptr<Logger> cur_logger = nullptr;

    #define LOGGER (cur_logger->get_stream())
}

#endif