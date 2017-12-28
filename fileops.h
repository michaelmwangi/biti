#ifndef FILEOPS_H
#define FILEOPS_H

#include <unistd.h> // pread()
#include <vector>
#include "file.h" 
#include "consts.h"


namespace biti{
    class FileOps{
        private:
            File &file;
            std::vector<std::string> split_buf();
        public:
            FileOps(File &file);
            void read_file();
            void evaluate();
    };    
}

#endif