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
        int num_read = pread(file.fd, store, BUF_SIZE, file.curpos);
        tot_read = num_read;
        if(bytesread < num_read){
            // seems like there ws more data to read than we anticipated lets adjust
            bytesread = num_read;
        }
        while(tot_read <= bytesread){

            if (num_read <= 0){
                if(num_read == -1){
                    LOGGER->log(LogLevel::ERROR, "Could not read %s successfully", LOGGER->error_no_msg());
                }                
                break;
            }else{
                if (tot_read < bytesread){
                    // we still have more data to consume
                    std::fill(store, store+BUF_SIZE, '\0');
                    num_read = pread(file.fd, store,  BUF_SIZE, file.curpos);
                    tot_read += num_read;
                }
                
                // Hack .. some editors add an implicit newline at the end of the file such that the file ends up with \n\n
                // let's hunt for the extra one and remove it
                if(store[num_read-1] == '\n' && store[num_read-2] == '\n'){
                    store[num_read-1] = '\000';
                    tot_read -= 1;
                    bytesread -= 1;
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
        LOGGER->log(LogLevel::ERROR, "Size of new changes on file %s %s", file.fpath, std::to_string(delta_sz));
        if (delta_sz <= 0){            
            // no change detected
            LOGGER->log(LogLevel::DEBUG, "No changes detected on file %s", file.fpath);
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
            LOGGER->log(LogLevel::DEBUG, "Matching %s against %s in file %s", pat, token, file.fpath);
            auto pos = token.find(pat);
            if(pos == std::string::npos){
                LOGGER->log(LogLevel::DEBUG, "Match not found for pattern %s in file %s", pat, file.fpath);
            }else{
                LOGGER->log(LogLevel::DEBUG, "Match found for pattern %s in file %s proceeding to trigger attached backend", pat, file.fpath);
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
        LOGGER->log(LogLevel::DEBUG, "Getting the json state of file %s", file.fpath);
        return file.to_json();
    } 
}