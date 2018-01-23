#include "logger.h"

namespace biti {
   std::shared_ptr<Logger> Logger::get_current_logger(){
        assert(cur_logger != nullptr);
        return cur_logger;
   }

   FileLogger::FileLogger(const std::string &filepath, LogLevel loglevel):
        log_stream{filepath, std::ios::out | std::ios::app}, log_level{loglevel}
    {
        if(!log_stream.is_open()){
            // set error here
        }
    }

   void FileLogger::write(const std::string & data, LogLevel loglevel){
       if(log_level != LogLevel::NONE && (loglevel == log_level || log_level == LogLevel::DEBUG)){           
           log_stream << data <<std::endl;
       }

       if(loglevel == LogLevel::SEVERE){
           exit(1)
       }
   }
   
}