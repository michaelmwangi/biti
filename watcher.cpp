#include "watcher.h"
#include "fileops.h"

namespace biti {
    Watcher::Watcher(std::vector<std::string> &filepaths)
    {
        LOGGER->write("New starting up", LogLevel::INFO);
        inotify_fd = inotify_init();

        if(inotify_fd == -1){
            perror("inotify_init");
            exit(EXIT_FAILURE);
        }
        
        for(auto fpath : filepaths){
            // make sure we addd only regular files
            struct stat stbuf;
            int res = stat(fpath.c_str(), &stbuf);

            if (res == 0){
                if(! S_ISREG(stbuf.st_mode)){
                    std::cerr<<fpath<<" is not a regular file"<<std::endl;
                    continue;
                }
            }else{
                perror("stat");
                std::cerr<<"Could not get "<<fpath<<" properties"<<std::endl;
                continue;
            }

            int wd = inotify_add_watch(inotify_fd, fpath.c_str(), IN_MODIFY);

            if(wd == -1){
                std::cerr<<"Cannot watch "+fpath<<std::endl;
                perror("inotify_add_watch");
            }else{
                File fileobj(wd, fpath);
                store.insert(std::make_pair(wd, std::make_unique<FileOps>(std::move(fileobj))));            
            }                        
        }        
    }

    Watcher::~Watcher(){

        if(inotify_fd > 0){
            close(inotify_fd);
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
            // seems we were not able to watch any file lets throw and exc
            std::cerr<<"NO files to watch"<<std::endl;
        }else{

            // process files for data already in the file
            for (const auto &iter : store){
                iter.second->evaluate();
            }
            
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
                            fileop->evaluate();
                            store[_wd] = std::move(fileop);
                        }else{
                            std::cerr<<"Descriptor "<<_wd<<" not found in store"<<std::endl;
                        }

                    }
                }
            }
        }
        
    }
}