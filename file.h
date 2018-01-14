#ifndef FILE_H
#define FILE_H

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "consts.h"

namespace biti{
    struct File {
        int fd; // the open file descriptor
        int wd; // the watch file desc // inotify stuff        
        int curPos; // the current file position
        int err; // the last error number 
        int size; // the size of the file since our last read
        std::string fPath; // the file path)
        std::string delimeter; 
        std::string buf; // the current data we have extracted from file but not yet processed

        File(int wd, std::string path, std::string del=","){
            wd = wd;
            fPath = path;
            curPos = 0;
            err = 0;
            size = 0;
            delimeter = del;            

            int f_desc = open(fPath.c_str(), O_RDONLY);
            if(f_desc == -1){
                perror("Open");
                exit(EXIT_FAILURE);
                // we should probably just skip this file and move on with other files
            }else{
                fd=f_desc;
                buf.resize(BUF_SIZE);
            }            
        }
    };
}


#endif