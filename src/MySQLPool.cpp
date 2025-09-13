#include "MySQLPool.h"
#include <mysql/mysql.h>
#include <spdlog/spdlog.h>

MySQLPool::MySQLPool(size_t pool_size,
                             const std::string& host,
                             const std::string& user,
                             const std::string& passwd,
                             const std::string& db,
                             unsigned int port)
{
    for (size_t i = 0; i < pool_size; ++i) {
        MYSQL* conn = mysql_init(nullptr);
        if (!mysql_real_connect(conn, host.c_str(), user.c_str(), passwd.c_str(), db.c_str(), port, nullptr, 0)) {
            spdlog::error("MySQL connect error: {}", mysql_error(conn));
            mysql_close(conn);
            continue;
        }
        connections_.push(conn);
    }
}

MySQLPool::~MySQLPool() {
    std::lock_guard<std::mutex> lock(mutex_);
    while (!connections_.empty()) {
        mysql_close(static_cast<MYSQL*>(connections_.front()));
        connections_.pop();
    }
}

std::future<DbResult> MySQLPool::executeAsync(const std::string& sql) {
    std::promise<DbResult> prom;
    auto fut = prom.get_future();

    std::thread([this, sql, prom = std::move(prom)]() mutable {
        DbResult result;
        MYSQL* conn = static_cast<MYSQL*>(acquireConnection());
        if (!conn) {
            result.error = "No available connection";
            prom.set_value(result);
            return;
        }

        if (mysql_query(conn, sql.c_str()) != 0) {
            result.error = mysql_error(conn);
        } else {
            MYSQL_RES* res = mysql_store_result(conn);
            if (res) { // SELECT
                result.isQuery = true;
                MYSQL_ROW row;
                MYSQL_FIELD* fields = mysql_fetch_fields(res);
                unsigned int num_fields = mysql_num_fields(res);

                while ((row = mysql_fetch_row(res))) {
                    std::unordered_map<std::string, std::string> map;
                    for (unsigned int i = 0; i < num_fields; ++i) {
                        map[fields[i].name] = row[i] ? row[i] : "";
                    }
                    result.rows.push_back(std::move(map));
                }
                mysql_free_result(res);
            } else { // INSERT/UPDATE/DELETE
                result.affectedRows = mysql_affected_rows(conn);
            }
        }

        releaseConnection(conn);
        prom.set_value(std::move(result));
    }).detach();

    return fut;
}

MYSQL* MySQLPool::acquireConnection() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (connections_.empty()) return nullptr;
    MYSQL*  conn = connections_.front();
    connections_.pop();
    return conn;
}

void MySQLPool::releaseConnection(MYSQL*  conn) {
    std::unique_lock<std::mutex> lock(mutex_);
    connections_.push(conn);
}