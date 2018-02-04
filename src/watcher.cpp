#include "watcher.h"

namespace biti {
    Watcher::Watcher(Config &config)
    {
        inotify_fd = inotify_init();
        if(inotify_fd == -1){
            perror("inotify_init");
            exit(1);
        }
        auto fobjs = config.get_file_configs();
        for(auto fobj : fobjs){
            // make sure we add only regular files
            struct stat stbuf;
            int res = stat( fobj.fpath.c_str(), &stbuf);
            if (res == 0){
                if(! S_ISREG(stbuf.st_mode)){
                    LOGGER->write(fobj.fpath+" is not a regular file .. ignoring!", LogLevel::ERROR);                    
                    continue;
                }
            }else{                
                LOGGER->write("Could not get "+fobj.fpath+" properties", LogLevel::ERROR);
                continue;
            }

            int wd = inotify_add_watch(inotify_fd, fobj.fpath.c_str(), IN_MODIFY);

            if(wd == -1){
                LOGGER->write("Cannot watch "+fobj.fpath+" : "+strerror(errno), LogLevel::ERROR);
            }else{                
                LOGGER->write("Adding "+fobj.fpath+" to the watch list", LogLevel::INFO);
                store.insert(std::make_pair(wd, std::make_unique<FileOps>(std::move(fobj))));                            
            }                        
        }        
    }

    Watcher::~Watcher(){

        if(inotify_fd > 0){
            for(auto& it : store){
                inotify_rm_watch(it.second->get_file_fd(), it.first);
            }
            close(inotify_fd);
        }
    }
    /*
       process task from the task queue
    */
    void Watcher::process_bg_tasks(){
        while(true){
            auto task = task_queue.get_task();
            switch(task.type){
                case FILE_SAVE:
                    // saving file to disk, arg_1 -> file name , arg_2-> file contents
                    
                default:
                    // I dont know what to do with this task
            }
        }
    }

    /*
        prepare the db backup file and set the file descriptor
    */
    void Watcher::init_db(std::string dbfile){
        db_file_fd = open(dbfile, O_WRONLY|O_APPEND|O_CREATE, 0644);
        if(db_file_fd == -1){
            LOGGER->write("Could not initialize db file %s due to %s", dbfile, strerr(errno),
                          LogLevel::SEVERE);
        }
    }

    void Watcher::watch(){
        
        // TODO
        /*
            1. What if the file is not a text file
            2. What if the file is deleted while we are watching it 
            3. What if we add a content at any other place other than the tail end of the file
            4. What if we start watching a file which already has data
            5. What if the file that we are watching is gets log rotated
            5. What if the file we are watching changes modes
            6. How do we store our current state in case we crash or we are restarted            
        */
        if(store.empty()){
            LOGGER->write("There are no files to watch!!", LogLevel::SEVERE);
        }else{
            LOGGER->write("Starting the nights watch", LogLevel::DEBUG);
            // process files for data already in the file
            for (const auto &iter : store){
                iter.second->evaluate();
            }
            
            // launch the background processing thread 
            // TODO provide a way to cancel tasks 
            auto fut = std::async(std::launch::async, process_bg_tasks);

            // wait for new events
            // The loop blocks on read() call until we have an event available on the inotify_fd 
            while(true){
                char buf[BUF_SIZE] = {};
                int len = read(inotify_fd, buf, BUF_SIZE);
                if(len <= 0){
                    continue;
                }
                const struct inotify_event *evt;
                for(char *ptr=buf;ptr < buf+len;ptr += sizeof(struct inotify_event)+evt->len){
                    evt = reinterpret_cast<inotify_event *>(ptr);
                    if((evt->mask & IN_MODIFY) == IN_MODIFY){                        
                        int _wd = evt->wd;
                        auto it = store.find(_wd);
                        if(it != store.end()){
                            auto fileop = std::move(it->second);
                            LOGGER->write("Changes detected in "+fileop->get_file_name()+" proceeding to evaluate", LogLevel::DEBUG);
                            fileop->evaluate();
                            LOGGER->write("Finished evaluating "+fileop->get_file_name(), LogLevel::DEBUG);
                            store[_wd] = std::move(fileop);
                        }else{
                            LOGGER->write("Watch descriptor "+std::to_string(_wd)+" was not found in store ", LogLevel::ERROR);
                        }

                    }
                }
            }
        }
        
    }
}