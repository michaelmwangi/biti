#ifndef FILE_H
#define FILE_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <vector>
#include <cstring>
#include <sstream>
#include "json.hpp"
#include "consts.h"
#include "logger.h"


namespace biti{
    using json = nlohmann::json;
    struct File {
        int fd; // the open file descriptor
        int curpos; // the current file position                
        int size; // the size of the file since our last read
        int err; // the last error number // a error number of zero means that evrythinhg is good 
        std::string errmsg; // the last error message we got while operating on this file
        std::string fpath; // the file path)
        std::string delimeter; // the delimeter that separates the tokens we want to pick eg new line (\n) 
        std::vector<std::string> patterns; // patterns that we are matching the tokens against
        std::string buf; // the current data we have extracted from file but not yet processed

        File(std::string path, std::string del, std::vector<std::string> &patterns):
            fpath{path}, curpos{0}, err{0}, size{0}, delimeter{del}, patterns {patterns}
        {        
            int f_desc = open(fpath.c_str(), O_RDONLY);
            if(f_desc == -1){                
                LOGGER->log(LogLevel::ERROR, "Unable to open file %s due to %s", path, LOGGER->error_no_msg());
            }else{
                fd = f_desc;
                buf.reserve(BUF_SIZE);
            }            
        }

        /*
            Serlializes the curent state of the file object into a json object
        */
        json to_json(){
            json state;
            state["fd"] = fd;
            state["curpos"] = curpos;
            state["size"] = size;
            state["err"] = err;
            state["errmsg"] = errmsg;
            state["fpath"] = fpath;
            return state;
        }

    };
}


#endif