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
            LOGGER->log(LogLevel::ERROR, "Could not create a snapshot contents due to %s", LOGGER->error_no_msg());
            return false;
        }
        int num = write(fd, content.c_str(), content.length());
        if(num != content.length()){
            LOGGER->log(LogLevel::ERROR, "Could not reliably write all bytes of the snapshot due to %s", LOGGER->error_no_msg());
            remove_tmp_file = true;
        }else{
            int save = fsync(fd);
            if(save == -1){
                LOGGER->log(LogLevel::ERROR, "Could not persist DB snapshot data to disk due to %s", LOGGER->error_no_msg());
                remove_tmp_file = true;
            }
            close(fd);
        }
        
        //atomically rename the file
        int res = rename(tmp_fname.c_str(), db_name.c_str());
        if(res != 0){
            LOGGER->log(LogLevel::ERROR, "Could not rename new snapshot Db from %s to %s due to %s", tmp_fname, db_name, LOGGER->error_no_msg());
            remove_tmp_file = true;
        }
        
        if(remove_tmp_file){
              // delete tmp file 
            int res = remove(tmp_fname.c_str());
            if(res == 0){
                LOGGER->log(LogLevel::INFO, "Successfully removed temporary DB file after failing to rename");
            }else{
                LOGGER->log(LogLevel::ERROR, "Could not remove temporary DB file due to %s", LOGGER->error_no_msg());
            }
            return false;
        }else{
            LOGGER->log(LogLevel::DEBUG, "Successfully created biti DB snapshopt");
            return true;
        }
    }
}

#endif