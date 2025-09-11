#include "ConfigLoader.h"
#include <yaml-cpp/yaml.h>
#include <iostream>

AppConfig ConfigLoader::load(const std::string& file) {
    AppConfig cfg;
    try {
        YAML::Node root = YAML::LoadFile(file);
        auto db = root["database"];
        cfg.db.pool_size = db["pool_size"].as<std::size_t>();
        cfg.db.host      = db["host"].as<std::string>();
        cfg.db.user      = db["user"].as<std::string>();
        cfg.db.password  = db["password"].as<std::string>();
        cfg.db.name      = db["name"].as<std::string>();

        auto log = root["logging"];
        cfg.log.level = log["level"].as<std::string>();
        cfg.log.file  = log["file"].as<std::string>();
    } catch (const std::exception& e) {
        std::cerr << "Error loading config: " << e.what() << std::endl;
        throw;
    }
    return cfg;
}