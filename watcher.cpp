#include "watcher.h"
#include "fileops.h"

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
                std::cerr<<"Cannot watch "+fpath<<std::endl;
                perror("inotify_add_watch");
            }else{
                store.insert(std::make_pair(wd, std::make_shared<File>(wd, fpath)));            
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
            6. How do we store our current state in case we crash or we are restarted            
        */
        if(store.empty()){
            // seems we were not able to watch any file lets throw and exc
            std::cerr<<"NO files to watch"<<std::endl;
        }else{
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
                            auto fileptr = it->second;
                            FileOps fileops(fileptr);
                            fileops.evaluate();
                        }else{
                            std::cerr<<"Descriptor "<<_fd<<" not found in store"<<std::endl;
                        }

                    }
                }
            }
        }
        
    }
}