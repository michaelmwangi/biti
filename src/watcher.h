#ifndef WATCHER_H
#define WATCHER_H

#include <vector>
#include <string>
#include <memory>
#include <future>
#include <iostream>
#include <cstring>
#include <unistd.h> 
#include <poll.h>
#include <ctime>
#include <unordered_map>
#include <sys/inotify.h>
#include <sys/stat.h>
#include "config.h"
#include "consts.h"
#include "file.h"
#include "fileops.h"
#include "logger.h"
#include "taskqueue.h"
#include "db/snapshot_db.h"

namespace biti {
    class Watcher {
        private:
            int inotify_fd;
            int db_file_fd; 
            int save_time_ms; // time interval in ms to save to disk
            TaskQueue task_queue;
            // TODO consider using a denser map implementation as the std::unordered makes use of a linked list hence 
            // missing cache (cache misses)
            std::unordered_map<int, std::unique_ptr<FileOps>> store;
            void init_db(std::string );
        public:
            Watcher(Config &config);
            ~Watcher();
            void watch();
            void process_bg_tasks();
    };
}


#endif