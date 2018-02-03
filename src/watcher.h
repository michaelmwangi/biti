#ifndef WATCHER_H
#define WATCHER_H

#include <vector>
#include <string>
#include <memory>
#include <future>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <unordered_map>
#include <sys/inotify.h>
#include <sys/stat.h>
#include "config.h"
#include "consts.h"
#include "file.h"
#include "fileops.h"
#include "logger.h"
#include "taskqueue.h"

namespace biti {
    class Watcher {
        private:
            int inotify_fd;
            TaskQueue task_queue;
            std::unordered_map<int, std::unique_ptr<FileOps>> store;
        public:
            Watcher(Config &config);
            ~Watcher();
            void watch();
            void process_bg_tasks();
    };
}


#endif