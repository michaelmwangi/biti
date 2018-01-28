#ifndef FILE_H
#define FILE_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <sstream>
#include "consts.h"
#include "logger.h"


namespace biti{
    struct File {
        int fd; // the open file descriptor
        int wd; // the watch file desc // inotify stuff        
        int curpos; // the current file position                
        int size; // the size of the file since our last read
        int err; // the last error number // a error number of zero means that evrythinhg is good 
        std::string errmsg; // the last error message we got while operating on this file
        std::string fpath; // the file path)
        std::string delimeter; // the delimeter that separates the tokens we want to pick eg new line (\n) 
        std::vector<std::string> patterns; // patterns that we are matching the tokens against
        std::string buf; // the current data we have extracted from file but not yet processed

        File(std::string path, std::string del, std::vector<std::string> &patterns):
            wd{-1},fpath{path}, curpos{0}, err{0}, size{0}, delimeter{del}, patterns {patterns}
        {        
            int f_desc = open(fpath.c_str(), O_RDONLY);
            if(f_desc == -1){
                std::ostringstream msg;
                msg << "Unable to open file "<<path<<" due to "<<std::strerror(errno);
                err = errno;
                errmsg = msg.str();
                LOGGER->write(msg.str(), LogLevel::ERROR);
            }else{
                fd=f_desc;
                buf.resize(BUF_SIZE);
            }            
        }
    };
}


#endif