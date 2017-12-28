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


int main(){    
    std::vector<std::string> fpaths = {"/mnt/programming/mike.txt"};
    biti::Watcher watcher(fpaths);
    watcher.watch();
    return 0;
}