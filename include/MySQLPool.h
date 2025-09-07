#pragma once
#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <future>
#include <mutex>
#include <queue>

struct DbResult {
    bool isQuery = false;  // true: SELECT, false: INSERT/UPDATE/DELETE
    std::vector<std::unordered_map<std::string, std::string>> rows;
    my_ulonglong affectedRows = 0;
    std::string error;
};

class MySQLPool {
public:
    MySQLPool(size_t pool_size, const std::string& host,
              const std::string& user, const std::string& passwd,
              const std::string& db);
    ~MySQLPool();

    std::future<DbResult> executeAsync(const std::string& sql);

private:
    MYSQL* getConnection();
    void releaseConnection(MYSQL* conn);

private:
    std::vector<MYSQL*> connections_;
    std::queue<MYSQL*> freeConnections_;
    std::mutex mutex_;
};