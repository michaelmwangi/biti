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
            std::string conf_filename;            
            int save_time_ms;         
            std::string logfile; // name of the biti logfile
            std::string dbfile; // name of file which we will use as a DB
            std::map<std::string, std::string> known_delimeters; // a mapping of known delimeters that commonly used such as newlines, commas 
            template<typename T>
            T get_item(const json &jconfig, std::string item);
            void process();
        public:
            Config(std::string & fname);                   
            std::string get_log_path();
            std::string get_db_path();
            int get_save_time();
            std::vector<File> &get_file_configs();
    };
}

#endif