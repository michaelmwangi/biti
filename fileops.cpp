#include "fileops.h"
#include <iostream>
#include <fcntl.h>

namespace biti {
    FileOps::FileOps(File file):
        file(file)
        {}
    
    /**
     * Reads upto bytesread from the current file offset    
     * we will read upto this much of bytes from the file. If the file size increases as we are reading it will be caught in the next
     * inotify IN_MODIFY event and so on
     * */
    void FileOps::read_file(int bytesread){        
        char store[BUF_SIZE] = {};
        int tot_read = 0;
        int num_read = pread(file.fd, store,  BUF_SIZE, file.curPos);
        tot_read = num_read;
        while(tot_read <= bytesread){
            if (num_read <= 0){
                if(num_read == -1){
                    perror("pread");
                }
                
                break;
            }else{
                if (tot_read < bytesread){
                    // we still have more data to consume
                    std::fill(store, store+BUF_SIZE, '\0');
                    num_read = pread(file.fd, store,  BUF_SIZE, file.curPos);
                    tot_read += num_read;
                }

                file.buf += store; 
                file.curPos += tot_read;   

                if (tot_read == bytesread){
                    break;
                }
            }
        }
        
    }

    void FileOps::evaluate(){
        // split the buffer string into segments separated by the set delimeter
        // match each segement with the set pattern and trigger the attached backend
        // after matching each segment discard it 
        int delta_sz = get_file_size_delta();
        if (delta_sz < 0){
            // no change detected
            return;
        }
        read_file(delta_sz);
        auto tokens = split_buf();
        for(auto token : tokens){
            // pattern match and trigger backend if found

            // finally purge token stem from buffer
            int pos = file.buf.find(token);
            if(pos != std::string::npos){
                file.buf.replace(pos, token.size()+1, "");                
                std::cout<<token<<std::endl;
            }else{
                // log error here we really do not expect to land here
                std::cerr<<"How did I get here!!"<<std::endl;
            }                                         
        }
        
        // std::cout<<"The final buf "<<file.buf<<std::endl;
    }

    // splits the buffer into tokens according to the delimeter passed
    std::vector<std::string> FileOps::split_buf(){
        std::vector<std::string> tokens;
        int start = 0;
        int end = file.buf.find(file.delimeter);
        while(end != std::string::npos){
            tokens.emplace_back(file.buf.substr(start, end-start));
            start = end+1;
            end = file.buf.find(file.delimeter, start);
        }
        return tokens;
    }
    
    int FileOps::get_file_size_delta(){
        int sz = lseek(file.fd, 0, SEEK_END);
        if (sz == -1){
            return -1;
        }else{
            if (sz == 0){
                return 0;
            }else{                
                int delta = 0;
                if(sz < file.size){
                    // file was truncated                    
                    file.curPos = 0;
                    delta = sz;
                }else{
                    delta = sz - file.size;
                }          
                file.size = sz;
                return delta;      
            }            
        }
    }
}