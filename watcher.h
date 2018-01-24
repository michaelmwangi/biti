#ifndef WATCHER_H
#define WATCHER_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <cstring>
#include <unistd.h> // close() read()
#include <unordered_map>
#include <sys/inotify.h>
#include <sys/stat.h>
#include "config.h"
#include "consts.h"
#include "file.h"
#include "fileops.h"
#include "logger.h"

namespace biti {
    class Watcher {
        private:
            int inotify_fd;
            std::unordered_map<int, std::unique_ptr<FileOps>> store;
        public:
            Watcher(Config &config);
            ~Watcher();
            void watch();
    };
}


#endif