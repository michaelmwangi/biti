#include "watcher.h"

namespace biti {
    Watcher::Watcher(Config &config):
        config {config}
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
            std::string contents;
            bool state = false;
            switch(task.type){
                case TaskType::FILE_SAVE:
                    LOGGER->write("Saving file to disk", LogLevel::DEBUG);
                    contents = task.arg;
                    state = create_snapshot(config.get_db_path(), contents);
                default:
                    // I dont know what to do with this task
                    LOGGER->write("Received unknown task....", LogLevel::WARNING);
            }
        }
    }

    void init_from_db(){
        std::ifstream dbfile(config.get_db_path(), std::ifstream::read);
        //get file lenght
        // dbfile.seekg(0, dbfile.end);
        // int len = dbfile.tellg();
        // dbfile.seekg(0, dbfile.beg);

        // dbfile.read()
        
        // TODO what if db file does not exist
        
        json db_data;
        
        try{
            dbfile >> db_data;
        }catch (json::parse_error &e){
            std::stringstream err;
            err << e.what();
            LOGGER->write("Cannot read db file "+config.get_db_path()+" "+err.str(), LogLevel::ERROR);
            // remove corrupt db file
            int res = remove(config.get_db_path().c_str());
            if(res == 0){
                LOGGER->write("Successfully removed corrupt DB file "+config.get_db_path(), LogLevel::INFO);
            }else{
                LOGGER->write("Could not remove corrupt db file due to "+std::string(strerror(errno))    , LogLevel::ERROR);
            }
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
            LOGGER->write("Starting the nights watch", LogLevel::INFO);
            
            // process files for data already in the file
            for (const auto &iter : store){
                iter.second->evaluate();
            }
            
            // launch the background processing thread 
            // TODO provide a way to cancel tasks 
            auto fut = std::async(std::launch::async, &Watcher::process_bg_tasks, this);
            
            // start of event loop
            // we are using poll since its better than select and not using epoll variants as we are basically
            // only watching one descriptor (inotify fd) thus making the code simple
            struct pollfd poll_fds;
            while(true){
                
                poll_fds.fd = inotify_fd;
                poll_fds.events = POLLIN;
                int ready = poll(&poll_fds, 1, config.get_save_time());
                
                if(ready < 0){
                     // TODO I think we can handle this better 
                    LOGGER->write("Error waiting for events: "+LOGGER->error_no_msg(), LogLevel::ERROR);
                }else if(ready == 0){
                    // the timeout expired -- time to save our current state to disk
                    // push task to background worker (queue)
                    Task task;
                    biti::json payload;
                    for(auto &it : store){
                        biti::json snapshot = it.second->dump_file_snapshot();
                        std::string name = it.second->get_file_name();
                        payload[name] = snapshot;

                    }   
                    task.arg = payload.dump();
                    task.created = time(nullptr);
                    task.type = TaskType::FILE_SAVE;
                    task_queue.push(task);                    
                }else{
                    //TODO make sure we have a polling event    
                    // an inotify event is ready
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
}