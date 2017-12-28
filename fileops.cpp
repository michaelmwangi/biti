#include "fileops.h"
#include <iostream>

namespace biti {
    FileOps::FileOps(File &file):
        file(file)
        {}
    
    void FileOps::read_file(){
        char buf[BUF_SIZE];
        // TODO do checks on readability of file ..offset 
        int num_read = pread(file.fd, buf,  BUF_SIZE, file.curPos);
        if (num_read == -1){
            perror("pread");
        }else{
            if(num_read == 0){
                // we got nothing why did we even get the event notification ??? hmm
            }else{
                // TODO what if we still have more data to read
                file.buf += std::string(buf); 
                file.curPos += num_read;   
            }
        }
    }

    void FileOps::evaluate(){
        // split the buffer string into segments separated by the set delimeter
        // match each segement with the set pattern and trigger the attached backend
        // after matching each segment discard it 
        auto tokens = split_buf();
        for(auto token : tokens){
            // pattern match and trigger backend if found

            // finally purge token stem from buffer
            int pos = file.buf.find(token);
            if(pos != std::string::npos){
                file.buf.replace(pos, token.size(), "");
                std::cout<<token<<std::endl;
            }else{
                // log error here we really do not expect to land here
            }            
        }        
    }

    // splits the buffer into tokens according to the delimeter passed
    std::vector<std::string> FileOps::split_buf(){
        std::vector<std::string> tokens;
        int start = 0;
        int end = file.buf.find(file.delimeter);
        while(end != std::string::npos){
            tokens.emplace_back(file.buf.substr(start, end-start));
            start = end+1;
            end = file.buf.find(file.delimeter, end);
        }
        return tokens;
    }
    
}