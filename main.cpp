#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <sys/inotify.h>
#include <sys/epoll.h> // epoll stuf
#include <unistd.h> // close() read()
#include "consts.h"

#include <vector>
#include <string>
#include "file.h"
#include "watcher.h"
#include "logger.h"

namespace biti{
    std::shared_ptr<Logger> cur_logger = nullptr; 
}

int main(int argc, char **argv){    
    // This is just a stub we are not even going to do any error checking
    if (argc == 3){
        std::vector<std::string> fpaths = {argv[1]};
        std::string log_path = argv[2];
        biti::cur_logger = std::make_shared<biti::FileLogger>(log_path);
        biti::Watcher watcher(fpaths);
        watcher.watch();
    }else{
        std::cout<<"Please pass in the file path and log path as an argument in that order"<<std::endl;
    }
    
    return 0;
}