#include "logger.h"

namespace biti {
   std::shared_ptr<Logger> Logger::get_current_logger(){
        return cur_logger;
   }

   void Logger::create_file_logger(const std::string &fname){
        cur_logger = std::make_shared<FileLogger>(fname);
   }

   FileLogger::FileLogger(const std::string &filepath):
        log_stream{filepath, std::ios::out | std::ios::app}
    {
        if(!log_stream.is_open()){
            // set error here
        }
    }

   std::ostream &FileLogger::get_stream(){
       return log_stream;
    }
   
}