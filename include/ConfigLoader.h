#pragma once
#include <string>

struct DBConfig {
    int pool_size;
    std::string host;
    std::string user;
    std::string password;
    std::string name;
};

struct LogConfig {
    std::string level;
    std::string file;
};

struct AppConfig {
    DBConfig db;
    LogConfig log;
};

class ConfigLoader {
public:
    static AppConfig load(const std::string& file);
};