#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <memory>
#include <sys/inotify.h>
#include <sys/epoll.h> // epoll stuf
#include <unistd.h> // close() read()
#include "consts.h"

#include "file.h"

int main(){    
    int nfd = inotify_init();
    if (nfd == -1){
        perror("Inotify_init1");
        exit(EXIT_FAILURE);
    }

    std::string fname = "/mnt/programming/lox/c++/mike.txt";
    int wd = inotify_add_watch(nfd, fname.c_str(), IN_MODIFY);

    if(wd == -1){
        perror("inotify_add_watch");
        exit(EXIT_FAILURE);
    }
    std::map<int, std::unique_ptr<biti::File>> fStore;
    // auto f = std::make_unique<File>(wd, fname);
    fStore.insert(std::make_pair(wd, std::make_unique<biti::File>(wd, fname)));
    struct epoll_event event;
    struct epoll_event *events = new struct epoll_event[64];
    // event.data.fd = wd;
    event.events = EPOLLIN | EPOLLET;
    int efd = epoll_create(1);
    
    if(efd == -1){
        perror("epoll_create");
        exit(EXIT_FAILURE);
    }

    int s = epoll_ctl(efd, EPOLL_CTL_ADD, nfd, &event);

    while(true){
        std::cout<<"blocking"<<std::endl;
        int res = epoll_wait(efd, &event, 64, -1);

        if(res == -1){
            perror("epoll_wait");
            break;
        }

        std::cout<<"Something happened"<<std::endl;
        while(true){
            char buf[biti::BUF_SIZE];
            int nu = read(nfd, buf, sizeof buf);
            if(nu <= 0){                
                perror("read");
                break;
            }else{
                const struct inotify_event *evt;
                for(char *ptr=buf;ptr < buf+nu;ptr += sizeof(struct inotify_event)+evt->len){                    
                    evt = (const struct inotify_event *) ptr;
                    if(evt->mask & IN_MODIFY){                        
                        int _fd = evt->wd;
                        auto it = fStore.find(_fd);
                        if(it != fStore.end()){
                            auto fl = std::move(it->second);
                            std::cout<<fl->fd<<fl->fPath<<fl->curPos<<std::endl;
                            fStore[_fd] = std::move(fl);
                        }else{
                            std::cerr<<"Descriptor "<<_fd<<" not found in store"<<std::endl;
                        }

                    }
                }
                                
            }
            break;
        }
        

        
    }
    return 0;
}