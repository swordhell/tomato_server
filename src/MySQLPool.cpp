#include "MySQLPool.h"
#include <thread>
#include <iostream>
#include <algorithm>

MySQLPool::MySQLPool(size_t pool_size, const std::string& host,
                     const std::string& user, const std::string& passwd,
                     const std::string& db) {
    for (size_t i = 0; i < pool_size; ++i) {
        MYSQL* conn = mysql_init(nullptr);
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), 3306, nullptr, 0)) {
            std::cerr << "MySQL connect error: " << mysql_error(conn) << "\n";
            mysql_close(conn);
            continue;
        }
        connections_.push_back(conn);
        freeConnections_.push(conn);
    }
}

MySQLPool::~MySQLPool() {
    for (auto conn : connections_) mysql_close(conn);
}

MYSQL* MySQLPool::getConnection() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (freeConnections_.empty()) return nullptr;
    MYSQL* conn = freeConnections_.front();
    freeConnections_.pop();
    return conn;
}

void MySQLPool::releaseConnection(MYSQL* conn) {
    std::lock_guard<std::mutex> lock(mutex_);
    freeConnections_.push(conn);
}

std::future<DbResult> MySQLPool::executeAsync(const std::string& sql) {
    std::promise<DbResult> prom;
    auto fut = prom.get_future();

    std::thread([this, sql, prom = std::move(prom)]() mutable {
        DbResult result;
        MYSQL* conn = getConnection();
        if (!conn) {
            result.error = "No available connection";
            prom.set_value(result);
            return;
        }

        if (mysql_query(conn, sql.c_str()) != 0) {
            result.error = mysql_error(conn);
            releaseConnection(conn);
            prom.set_value(result);
            return;
        }

        MYSQL_RES* res = mysql_store_result(conn);
        if (res) { // SELECT 查询
            result.isQuery = true;
            int num_fields = mysql_num_fields(res);
            MYSQL_FIELD* fields = mysql_fetch_fields(res);
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res))) {
                std::unordered_map<std::string, std::string> rowMap;
                for (int i = 0; i < num_fields; ++i)
                    rowMap[fields[i].name] = row[i] ? row[i] : "";
                result.rows.push_back(std::move(rowMap));
            }
            mysql_free_result(res);
        } else { // INSERT/UPDATE/DELETE
            result.isQuery = false;
            result.affectedRows = mysql_affected_rows(conn);
        }

        releaseConnection(conn);
        prom.set_value(result);
    }).detach();

    return fut;
}