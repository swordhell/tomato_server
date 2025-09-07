#pragma once
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <future>

class MySQLPool {
public:
    MySQLPool(size_t pool_size, const std::string& host,
              const std::string& user, const std::string& passwd, const std::string& db);
    ~MySQLPool();

    std::future<std::string> queryAsync(const std::string& sql);

private:
    std::vector<MYSQL*> connections_;
};