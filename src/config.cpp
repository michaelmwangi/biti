#include "config.h"
#include <iostream>

namespace biti{

    Config::Config(std::string &fname){
        conf_filename = fname;    
        // fill map of known delimeters
        known_delimeters.insert(std::make_pair("NEWLINE", "\n"));
        known_delimeters.insert(std::make_pair("COMMA", ","));
        known_delimeters.insert(std::make_pair("SEMICOLON", ";"));
        known_delimeters.insert(std::make_pair("COLON", ":"));
        
        process(conf_filename);
    }


    /*
        Fetches a string item from the config and terminates the program if item is
        mandatory and absent
    */
    template<typename T>
    T Config::get_item(const json &jconfig, std::string item){
        T res;
        try{
            res = jconfig.at(item).get<T>();
        }catch (json::out_of_range &e){           
            LOGGER->write("Mandatory item missing from config file "+item, LogLevel::SEVERE);             
        }
        return res;
    }


    /*
        Extracts and stores the file config items from the json config file
    */
    void Config::process(std::string fname){

        std::ifstream config_file(fname);   
        json json_config;
        
        try{
            config_file >> json_config;
        }catch (json::parse_error &e){
             std::stringstream err;
            err << e.what();
            std::cerr<<err.str()<<std::endl;
            exit(1);
        }

        logfile = get_item<std::string>(json_config, "log_file");

        if(logfile.empty()){
                    std::cerr<<"Could not find logfile path and therefore cannot continue!"<<std::endl;
                    exit(1);
        }
        // initialize the logger 
        cur_logger = std::make_shared<biti::FileLogger>(logfile, biti::LogLevel::DEBUG);

        dbfile = get_item<std::string>(json_config, "db_file");
        save_time_ms = get_item<int>(json_config, "db_save_interval_ms");
        
        if(save_time_ms > MAX_SAVE_MS){            
            save_time_ms = MAX_SAVE_MS;
        }        

        json items;
        try{
            items = json_config.at("items");
        }catch(json::out_of_range &exc){
           LOGGER->write("Could not find items to work on cannot continue!", LogLevel::SEVERE);
        }
        
        int item_counter = 0;
        for(auto iter=items.begin();iter != items.end(); iter++){
            item_counter++;
            json fileconfig = *iter;
            std::string fpath; // the file path
            std::string del; // the file delimeter
            json pats; // patterns to match against
            try{
                fpath = fileconfig.at("file");
                del = fileconfig.at("delimeter");
                pats = fileconfig.at("patterns");
            }catch(json::out_of_range &exc){
                std::ostringstream msg;
                msg << "Cannot evaluate item "<<item_counter<<" due to missing mandatory parameters "<<exc.what();
                LOGGER->write(msg.str(), LogLevel::ERROR);                
                continue;
            }

            if(pats.empty()){
                LOGGER->write("Could not find patterns for file "+fpath, LogLevel::ERROR);
                continue;
            }

            std::vector<std::string> _pats;
            for(auto p_iter=pats.begin();p_iter != pats.end(); p_iter++){
                _pats.push_back(*p_iter);
            }
            auto it = known_delimeters.find(del);
            if(it != known_delimeters.end()){
                del = it->second;
            }
            file_configs.emplace_back(fpath, del, _pats);
        }

    }

    std::string Config::get_log_path(){
        return logfile;
    }

    std::string Config::get_db_path(){
        return dbfile;
    }

    std::vector<File> &Config::get_file_configs(){
        return file_configs;
    }

    int Config::get_save_time(){
        return save_time_ms;
    }
}