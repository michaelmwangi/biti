#include "logger.h"

namespace biti {
   std::shared_ptr<Logger> Logger::get_current_logger(){
        assert(cur_logger != nullptr);
        return cur_logger;
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