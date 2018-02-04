#include "config.h"
#include <iostream>

namespace biti{
    Config::Config(std::string &fname):
        config_file{fname}
    {
        if(!config_file.good()){
            std::cout<<"Cannot open/read config file "+fname<<std::endl;
            exit(1);
        }else{
            std::cout<<"Using config file "+fname<<std::endl;
            
            // fill map of known delimeters
            known_delimeters.insert(std::make_pair("NEWLINE", "\n"));
            known_delimeters.insert(std::make_pair("COMMA", ","));
            known_delimeters.insert(std::make_pair("SEMICOLON", ";"));
            known_delimeters.insert(std::make_pair("COLON", ":"));
        }
    }

    Config::~Config(){
        config_file.close();
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
            std::cerr<<"Could not find mandatory item "<<item<<" configured .. cannot continue!"<<std::endl;
            exit(1);            
        }
        return res;
    }


    /*
        Extracts and stores the file config items from the json config file
    */
    void Config::process(){
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
        dbfile = get_item<std::string>(json_config, "db_file");
        save_time_ms = get_item<int>(json_config, "db_save_interval_ms");
        // initialize the logger 
        cur_logger = std::make_shared<biti::FileLogger>(logfile, LogLevel::DEBUG); 

        if(save_time_ms > MAX_SAVE_MS){
            // LOGGER->write("Maximum allowed save interval is %d ms but was passed %d ", MAX_SAVE_MS, LogLevel::ERROR);
            // LOGGER->write("Setting save interval to %d ms instead",MAX_SAVE_MS,
            //                 LogLevel::WARNING);
            save_time_ms = MAX_SAVE_MS;
        }
        

        json items;
        try{
            items = json_config.at("items");
        }catch(json::out_of_range &exc){
            std::cerr<<"Could not find items to work on and therefore cannot continue!"<<std::endl;
            exit(1);
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
                cur_logger->write(msg.str(), LogLevel::ERROR);                
                continue;
            }

            if(pats.empty()){
                cur_logger->write("Could not find patterns for file "+fpath, LogLevel::ERROR);
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

    std::vector<File> &Config::get_file_configs(){
        return file_configs;
    }

    int Config::get_save_time(){
        return save_time_ms;
    }
}