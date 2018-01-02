#ifndef WATCHER_H
#define WATCHER_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <unistd.h> // close() read()
#include <unordered_map>
#include <sys/inotify.h>

#include "consts.h"
#include "file.h"

namespace biti {
    class Watcher {
        private:
            int inotify_fd;
            std::unordered_map<int, std::shared_ptr<File>> store;
        public:
            Watcher(std::vector<std::string>);
            void watch();
    };
}


#endif