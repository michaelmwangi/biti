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
       auto ptime = std::ctime(&ltime);
       if(log_level != LogLevel::NONE && (loglevel == log_level || log_level == LogLevel::DEBUG)){           
           log_stream <<"["<<ptime<<"]"<<" "<<"["<<get_log_level(loglevel)<<"]"<< data <<std::endl;
       }

       if(loglevel == LogLevel::SEVERE){
           exit(1);
       }
   }
   
   
}