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


int main(int argc, char **argv){    
    // This is just a stub we are not even going to do any error checking
    if (argc == 2){
        std::vector<std::string> fpaths = {argv[1]};
        biti::Watcher watcher(fpaths);
        watcher.watch();
    }else{
        std::cout<<"Please pass in the file path as an argument"<<std::endl;
    }
    
    return 0;
}