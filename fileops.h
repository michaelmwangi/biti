#ifndef FILEOPS_H
#define FILEOPS_H

#include <pread>
#include <vector>
#include "file.h" 
#include "consts.h"


namespace biti{
    class FileOps{
        private:
            File &file;
            std::vector split_buf(std::string);
        public:
            FileOps(File &file);
            void read_file();
            void evaluate();
    };    
}

#endif