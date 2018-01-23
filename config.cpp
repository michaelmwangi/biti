#include "config.h"


namespace biti{
    Config::Config(std::string &fname):
        config_file{fname}
    {
        if(!config_file.good()){
            LOGGER->write("Cannot open/read config file "+fname, LogLevel::SEVERE);
            exit(1);
        }else{
            LOGGER->write("Using config file "+fname, LogLevel::INFO);
        }
    }

    Config::~Config(){
        config_file.close();
    }

    void Config::process(){
        json j;
        config_file >> j;
    }
}