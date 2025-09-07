#include "MySQLPool.h"
#include <thread>
#include <iostream>

MySQLPool::MySQLPool(size_t pool_size, const std::string &host,
                     const std::string &user, const std::string &passwd, const std::string &db) {
    for (size_t i = 0; i < pool_size; ++i) {
        MYSQL *conn = mysql_init(nullptr);
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 3306, nullptr, 0)) {
            std::cerr << "MySQL connect error: " << mysql_error(conn) << "\n";
            mysql_close(conn);
            continue;
        }
        connections_.push_back(conn);
    }
}

MySQLPool::~MySQLPool() {
    for (auto conn: connections_) mysql_close(conn);
}

std::future<std::string> MySQLPool::queryAsync(const std::string &sql) {
    std::promise<std::string> prom;
    auto fut = prom.get_future();

    std::thread([this, sql, prom = std::move(prom)]() mutable {
        std::string result;
        if (!connections_.empty()) {
            MYSQL *conn = connections_[0];
            if (mysql_query(conn, sql.c_str()) == 0) {
                MYSQL_RES *res = mysql_store_result(conn);
                if (res) {
                    MYSQL_ROW row = mysql_fetch_row(res);
                    if (row && row[0]) result = row[0];
                    mysql_free_result(res);
                }
            } else {
                result = "MySQL error";
            }
        }
        prom.set_value(result);
    }).detach();

    return fut;
}