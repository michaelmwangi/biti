#include "config.h"


namespace biti{
    Config::Config(std::string &fname):
        config_file{fname}
    {
        if(!config_file.good()){
            LOGGER->write("Cannot open/read config file "+fname, LogLevel::SEVERE);
        }else{
            LOGGER->write("Using config file "+fname, LogLevel::INFO);
        }
    }

    Config::~Config(){
        config_file.close();
    }

    void Config::process(){
        json json_config;
        try{
            config_file >> json_config;
        }catch (json::parse_error &e){
            std::stringstream err;
            err << e.what() << e.byte;
            LOGGER->write(err.str(), LogLevel::SEVERE);
        }
        
        // json_config.
    }
}