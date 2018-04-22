#ifndef SNAPSHOT_DB_H
#define SNAPSHOT_DB_H

/*
This module is used to save the state into a file. Like redis snapshots 
it writes to a temporary file then uses rename(2) to atomically rename it
**/

#include <stdio.h>
#include <string>
#include <fstream>
#include "string.h"
#include "../logger.h"

namespace biti{

    inline bool create_snapshot(std::string db_name, std::string &content){
        std::string tmp_fname = db_name+"_temp";
        std::ofstream stream = std::ofstream(tmp_fname, std::ios::binary|std::ios::app);        

        stream<<content;

        if(!stream.good()){
            LOGGER->write("Could not create snapshot contents due to " +std::string(strerror(errno)), LogLevel::ERROR);
            return false;
        }

        stream.close();
        //atomically rename the file
        int res = rename(tmp_fname.c_str(), db_name.c_str());
        if(res != 0){
            LOGGER->write("Could not rename new snapshot from "+tmp_fname+" to "+db_name+" due to" +strerror(errno), LogLevel::ERROR);
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