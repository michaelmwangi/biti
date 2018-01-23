#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include "json.hpp"
#include "logger.h"

namespace biti{
    using json = nlohmann::json;

    class Config{
        private:
            std::ifstream config_file;
        public:
            Config(std::string & fname);
            ~Config();
            void process();
    };
}

#endif