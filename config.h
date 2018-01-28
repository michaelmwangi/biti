#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include "file.h"
#include "json.hpp"
#include "logger.h"


namespace biti{
    using json = nlohmann::json;

    class Config{
        private:
            std::vector<File> file_configs; // Configuration for each file we are watching
            std::ifstream config_file;            
            std::string logfile; // name of the biti logfile
            std::map<std::string, std::string> known_delimeters; // a mapping of known delimeters that commonly used such as newlines, commas 
        public:
            Config(std::string & fname);
            ~Config();            
            void process();
            std::string get_log_path();
            std::vector<File> &get_file_configs();
    };
}

#endif