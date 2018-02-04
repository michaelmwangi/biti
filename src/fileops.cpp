#include "fileops.h"
#include <iostream>
#include <fcntl.h>

namespace biti {
    FileOps::FileOps(File file):
        file(file)
        {}
    
    /**
     * Reads upto bytesread from the current file offset    
     * we will read upto this much of bytes from the file. If the file size increases as we are reading it will be caught in the next due to
     * inotify IN_MODIFY event and so on
     * */
    void FileOps::read_file(int bytesread){        
        char store[BUF_SIZE] = {};
        int tot_read = 0;
        int num_read = pread(file.fd, store,  BUF_SIZE, file.curpos);
        tot_read = num_read;
        while(tot_read <= bytesread){
            if (num_read <= 0){
                if(num_read == -1){
                    LOGGER->write("Could not successfully read "+file.fpath+" due to "+ std::strerror(errno), LogLevel::ERROR);
                }                
                break;
            }else{
                if (tot_read < bytesread){
                    // we still have more data to consume
                    std::fill(store, store+BUF_SIZE, '\0');
                    num_read = pread(file.fd, store,  BUF_SIZE, file.curpos);
                    tot_read += num_read;
                }
                file.buf += store; 
                file.curpos += tot_read;   
                if (tot_read == bytesread){
                    break;
                }
            }
        }
        
    }

    /*
        evaluates the file changes and checks for matching configured patterns. 
        if a pattern is matched trigger the configured backend
    */
    void FileOps::evaluate(){
        // split the buffer string into segments separated by the set delimeter
        // match each segement with the set pattern and trigger the attached backend
        // after matching each segment discard it 
        int delta_sz = get_file_size_delta();
        LOGGER->write("Size of new changes on file "+file.fpath+" "+std::to_string(delta_sz)+" bytes", LogLevel::DEBUG);
        if (delta_sz <= 0){            
            // no change detected
            LOGGER->write("No changes detected on file "+file.fpath, LogLevel::DEBUG);
            return;
        }
        read_file(delta_sz);
        auto tokens = split_buf();
        
        // loop through each token and do pattern match and finally purge the token from buffer
        for(auto token : tokens){
            // pattern match and trigger backend if found
            pattern_match(token);

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
    }

    /*
        Splits the file changes that we just read from the file into tokens according to the delimeter
        passed during configuration for this particular file
    */     
    std::vector<std::string> FileOps::split_buf(){
        std::vector<std::string> tokens;
        int start = 0;
        int end = file.buf.find(file.delimeter);
        while(end != std::string::npos){
            auto tok = file.buf.substr(start, end-start);
            if(tok != ""){
                tokens.emplace_back(tok);                                
            }
            start = end+1;
            end = file.buf.find(file.delimeter, start);
        }
        return tokens;
    }
    
    void FileOps::pattern_match(const std::string &token){
        for(auto &pat : file.patterns){
            LOGGER->write("Matching "+pat+" against "+token+" in file "+file.fpath, LogLevel::DEBUG);
            auto pos = token.find(pat);
            if(pos == std::string::npos){
                LOGGER->write("Match not found for pattern "+pat+" in file "+file.fpath, LogLevel::DEBUG);
            }else{
                LOGGER->write("Match found for pattern "+pat+" in file "+file.fpath+" triggering backend ", LogLevel::DEBUG);
            }
        }
    }

    /*
        Returns the file size change since the last read
    */
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
                    file.curpos = 0;
                    delta = sz;
                }else{
                    delta = sz - file.size;
                }          
                file.size = sz;
                return delta;      
            }            
        }
    }

    /*
        Get the name of the file we are watching
    */
    std::string FileOps::get_file_name(){
        return file.fpath;
    }

    /*
        Get the file descriptor number
    */
    int FileOps::get_file_fd(){
        return file.fd;
    }

    /*
        Triggers the file serialize function and treturns the json object that
        represents the necessary info for the current state of the file eg current file offset
    */
    json FileOps::dump_file_snapshot(){
        LOGGER->write("Getting the json state of file "+file.fpath, LogLevel::DEBUG);
        return file.to_json();
    }
}