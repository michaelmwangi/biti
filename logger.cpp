#include "logger.h"

namespace biti {
   std::shared_ptr<Logger> Logger::get_current_logger(){
        
        if(cur_logger == nullptr){
            std::cerr<<"Logger not set!! Cannot continue!"<<std::endl;
            exit(1);
        }

        return cur_logger;
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
       if(log_level != LogLevel::NONE && (loglevel == log_level || log_level == LogLevel::DEBUG)){           
           log_stream << data <<std::endl;
       }

       if(loglevel == LogLevel::SEVERE){
           exit(1);
       }
   }
   
}