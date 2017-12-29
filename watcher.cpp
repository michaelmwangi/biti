#include "watcher.h"

namespace biti {
    Watcher::Watcher(std::vector<std::string> filepaths){
        inotify_fd = inotify_init();

        if(inotify_fd == -1){
            perror("inotify_init");
            exit(EXIT_FAILURE);
        }
        
        for(auto fpath : filepaths){
            int wd = inotify_add_watch(inotify_fd, fpath.c_str(), IN_MODIFY);

            if(wd == -1){
                perror("inotify_add_watch");
            }
            
            store.insert(std::make_pair(wd, std::make_unique<File>(wd, fpath)));            
        }
    }

    void Watcher::watch(){
        
        // TODO
        /*
            1. What if the file is not a text file
            2. What if the file is deleted
            3. What if we add a content at any other place other than the tail end of the file
            4. What if we start watching a file which already has data
            5. What if the file that we are watching is gets log rotated
            6. How do we store our current state in case we crash or we are restarted            
        */
        
        // The loop blocks on read() call until we have an event available on the inotify_fd 
        while(true){
                char buf[BUF_SIZE];
                int len = read(inotify_fd, buf, sizeof buf);

                if(len <= 0){
                    continue;
                }

                const struct inotify_event *evt;
                for(char *ptr=buf;ptr < buf+len;ptr += sizeof(struct inotify_event)+evt->len){
                    evt = (const struct inotify_event *)ptr;
                    if(evt->mask & IN_MODIFY){                        
                        int _fd = evt->wd;
                        auto it = store.find(_fd);
                        if(it != store.end()){
                            auto fl = std::move(it->second);
                            store[_fd] = std::move(fl);
                        }else{
                            std::cerr<<"Descriptor "<<_fd<<" not found in store"<<std::endl;
                        }

                    }
                }
            }
    }
}