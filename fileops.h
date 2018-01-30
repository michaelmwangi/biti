#ifndef FILEOPS_H
#define FILEOPS_H

#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include <memory>
#include "file.h" 
#include "consts.h"
#include "json.hpp"

namespace biti{
    using json = nlohmann::json;
    class FileOps{
        private:
            File file;            
            std::vector<std::string> split_buf();            
            void read_file(int);
            void pattern_match(const std::string &);
        public:
            FileOps(File file);     
            void evaluate();
            std::string get_file_name();
            int get_file_size_delta();  
            int get_file_fd();  
            json dump_file_snapshot();        
    };    
}

#endif