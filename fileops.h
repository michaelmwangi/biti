#ifndef FILEOPS_H
#define FILEOPS_H

#include <unistd.h> // pread()
#include <vector>
#include <memory>
#include "file.h" 
#include "consts.h"


namespace biti{
    class FileOps{
        private:
            std::shared_ptr<File> file;
            void read_file();
            std::vector<std::string> split_buf();            
        public:
            FileOps(std::shared_ptr<File> file);            
            void evaluate();
    };    
}

#endif