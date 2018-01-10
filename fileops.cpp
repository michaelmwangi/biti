#include "fileops.h"
#include <iostream>

namespace biti {
    FileOps::FileOps(std::shared_ptr<File> file):
        file(file)
        {}
    
    /**
     * reads thefile from the last read and populates the file buf data structure
     * */
    void FileOps::read_file(long f_size){
        char store[BUF_SIZE] = {};
        // std::fill(buf, buf+BUF_SIZE, '\0'); // initialize the buf to zero
        // TODO do checks on readability of file ..offset 
        int num_read = pread(file->fd, store,  BUF_SIZE, file->curPos);
        while(file->curPos != f_size &&  file->curPos < f_size){
            if (num_read == -1){
                perror("pread");
                break;
            }else{
                if(num_read == 0){
                    // we got nothing why did we even get the event notification ??? hmm
                }else{
                    // TODO what if we still have more data to read
                    file->buf += store; 
                    file->curPos += num_read;   
                }
                num_read = pread(file->fd, store,  BUF_SIZE, file->curPos);
            }
        }
        
    }

    void FileOps::evaluate(){
        // split the buffer string into segments separated by the set delimeter
        // match each segement with the set pattern and trigger the attached backend
        // after matching each segment discard it 
        // get file size so that we know max bytes to read 
        long f_size = get_file_size();
        read_file(f_size);
        auto tokens = split_buf();
        for(auto token : tokens){
            // pattern match and trigger backend if found

            // finally purge token stem from buffer
            int pos = file->buf.find(token);
            if(pos != std::string::npos){
                file->buf.replace(pos, token.size()+1, "");                
                std::cout<<token<<std::endl;
            }else{
                // log error here we really do not expect to land here
                std::cerr<<"How did I get here!!"<<std::endl;
            }                                         
        }
        
        // std::cout<<"The final buf "<<file->buf<<std::endl;
    }

    // splits the buffer into tokens according to the delimeter passed
    std::vector<std::string> FileOps::split_buf(){
        std::vector<std::string> tokens;
        int start = 0;
        int end = file->buf.find(file->delimeter);
        while(end != std::string::npos){
            tokens.emplace_back(file->buf.substr(start, end-start));
            start = end+1;
            end = file->buf.find(file->delimeter, start);
        }
        return tokens;
    }
    
    long FileOps::get_file_size(){
        struct stat stat_buf;
        int len = fstat(file->fd, &stat_buf);
        if (len == 0){
            return stat_buf.st_size;
        }else{
            // an error occured
            perror("fstat");
            return -1;
        }
    }
}