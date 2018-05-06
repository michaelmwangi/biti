#ifndef SNAPSHOT_DB_H
#define SNAPSHOT_DB_H

/*
This module is used to save the state into a file. Like redis snapshots 
it writes to a temporary file then uses rename(2) to atomically rename it
**/

#include <stdio.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "../logger.h"

namespace biti{

    inline bool create_snapshot(std::string db_name, std::string &content){
        std::string tmp_fname = db_name+"_temp";
        int fd = open(tmp_fname.c_str(), O_CREAT|O_WRONLY, 0644);
        bool remove_tmp_file = false;
        if(fd == -1){
            LOGGER->write("Could not create snapshot contents due to " +std::string(strerror(errno)), LogLevel::ERROR);
            return false;
        }
        int num = write(fd, content.c_str(), content.length());
        if(num != content.length()){
            LOGGER->write("Could not reliably write all bytes of the snapshot due to "+std::string(strerror(errno)), LogLevel::ERROR);
            remove_tmp_file = true;
        }else{
            int save = fsync(fd);
            if(save == -1){
                LOGGER->write("Could not persist db snapshot data to disk due to "+std::string(strerror(errno)), LogLevel::ERROR);
                remove_tmp_file = true;
            }
            close(fd);
        }
        
        //atomically rename the file
        int res = rename(tmp_fname.c_str(), db_name.c_str());
        if(res != 0){
            LOGGER->write("Could not rename new snapshot from "+tmp_fname+" to "+db_name+" due to" +strerror(errno), LogLevel::ERROR);
            remove_tmp_file = true;
        }
        
        if(remove_tmp_file){
              // delete tmp file 
            int res = remove(tmp_fname.c_str());
            if(res == 0){
                LOGGER->write("Successfully removed temporary snapshot file after failing to rename", LogLevel::INFO);
            }else{
                LOGGER->write("Could not remove temporary file due to "+std::string(strerror(errno))    , LogLevel::ERROR);
            }
            return false;
        }else{
            LOGGER->write("Successfully created biti snapshot", LogLevel::INFO);
            return true;
        }
    }
}

#endif