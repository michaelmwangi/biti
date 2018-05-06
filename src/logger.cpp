#include "logger.h"

namespace biti {
   std::shared_ptr<Logger> Logger::get_current_logger(){
        
        if(cur_logger == nullptr){
            std::cerr<<"Logger not set!! Cannot continue!"<<std::endl;
            exit(1);
        }

        return cur_logger;
   }
      
   std::string Logger::get_log_level(LogLevel loglevel){
       switch(loglevel){
            case LogLevel::INFO:
                return "INFO";
            case LogLevel::DEBUG:
                return "DEBUG";
            case LogLevel::WARNING:
                return "WARNING";
            case LogLevel::ERROR:
                return "ERROR";
            case LogLevel::SEVERE:
                return "SEVERE";
            case LogLevel::NONE:
                return "NONE";
            default:
                return "";
       }
   }

   FileLogger::FileLogger(const std::string &filepath, LogLevel loglevel):
        log_stream{filepath, std::fstream::out | std::fstream::app}, log_level{loglevel}
    {
        if(!log_stream.good()){
            // set error here
            std::cerr<<"Could not open logfile "<<filepath<<" and cannot continue!! "<<std::endl;
            std::cerr<<std::strerror(errno)<<std::endl;
            exit(1);
        }
    }

   void FileLogger::write(const std::string & data, LogLevel loglevel){
       auto now = std::chrono::system_clock::now();
       std::time_t ltime = std::chrono::system_clock::to_time_t(now);
       std::string ptime = std::ctime(&ltime);       
       ptime = ptime.replace(ptime.size()-1, 1, ""); // remove the newline
       if(log_level != LogLevel::NONE && (loglevel == log_level || log_level == LogLevel::DEBUG)){           
           log_stream <<"["<<ptime<<"]"<<" "<<"["<<get_log_level(loglevel)<<"] "<< data <<std::endl;
       }

       if(loglevel == LogLevel::SEVERE){
           exit(1);
       }
   }
   
   void FileLogger::log(LogLevel loglevel, const char *fmt, ...){
       auto now = std::chrono::system_clock::now();
       std::time_t ltime = std::chrono::system_clock::to_time_t(now);
       std::string ptime = std::ctime(&ltime);       
       ptime = ptime.replace(ptime.size()-1, 1, ""); // remove the newline

       char log_buf[LOG_MAX_LEN];
       va_list args;
       va_start(args, fmt);
       vsnprintf(log_buf, LOG_MAX_LEN, fmt, args);
       va_end(args);
       if(log_level != LogLevel::NONE && (loglevel == log_level || log_level == LogLevel::DEBUG)){           
           log_stream <<"["<<ptime<<"]"<<" "<<"["<<get_log_level(loglevel)<<"] "<< log_buf <<std::endl;
       }

       if(loglevel == LogLevel::SEVERE){
           exit(1);
       }
   }

   
}