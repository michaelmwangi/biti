#include <iostream>
#include <fstream>
#include "watcher.h"
#include "logger.h"
#include "config.h"

namespace biti{
    std::shared_ptr<Logger> cur_logger = nullptr; 
}

int main(int argc, char **argv){    
    // This is just a stub we are not even going to do any error checking
    if (argc == 2){
        std::string conf_path(argv[1]);
        biti::Config config(conf_path);
        biti::Watcher watcher(config);
        watcher.watch();                
    }else{
        std::cout<<"Please pass in the config file"<<std::endl;
    }
    
    return 0;
}