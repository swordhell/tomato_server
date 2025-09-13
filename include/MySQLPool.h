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
    MySQLPool(size_t pool_size,
                  const std::string& host,
                  const std::string& user,
                  const std::string& passwd,
                  const std::string& db,
                  unsigned int port = 3306);

    ~MySQLPool();

    // 异步执行 SQL
    std::future<DbResult> executeAsync(const std::string& sql);

private:
    MYSQL* acquireConnection();
    void releaseConnection(MYSQL* conn);

    std::queue<MYSQL*> connections_;
    std::mutex mutex_;
};