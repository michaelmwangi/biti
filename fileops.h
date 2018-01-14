#ifndef FILEOPS_H
#define FILEOPS_H

#include <unistd.h> // pread()
#include <vector>
#include <memory>
#include "file.h" 
#include "consts.h"
#include <sys/stat.h>

namespace biti{
    class FileOps{
        private:
            File file;            
            std::vector<std::string> split_buf();            
            void read_file(int);
        public:
            FileOps(File file);     
            void evaluate();
            int get_file_size_delta();
    };    
}

#endif