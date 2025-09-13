#include "gtest/gtest.h"
#include "MySQLPool.h"
#include "yaml-cpp/yaml.h"
#include <chrono>
#include <iostream>
#include "yaml-cpp/yaml.h"
#include <string>
#include <fstream>

struct DbConfig {
    std::string host;
    unsigned int port;
    std::string user;
    std::string password;
    std::string database;
    size_t pool_size;
};

DbConfig loadDbConfig(const std::string& filename) {
    YAML::Node config = YAML::LoadFile(filename);
    DbConfig db;
    db.host = config["host"].as<std::string>();
    db.port = config["port"].as<unsigned int>();
    db.user = config["user"].as<std::string>();
    db.password = config["password"].as<std::string>();
    db.database = config["database"].as<std::string>();
    db.pool_size = config["pool_size"].as<size_t>();
    return db;
}

// 读取配置
DbConfig dbConfig = loadDbConfig("db_config.yaml");

// 测试 MySQL 连接是否成功
TEST(MySQLPoolTest, ConnectTest) {
    MySQLPool pool(
        dbConfig.pool_size,
        dbConfig.host,
        dbConfig.user,
        dbConfig.password,
        dbConfig.database,
        dbConfig.port
    );

    auto fut = pool.executeAsync("SELECT 1");
    DbResult result = fut.get();

    EXPECT_TRUE(result.error.empty());
    EXPECT_TRUE(result.isQuery);
    ASSERT_FALSE(result.rows.empty());
    EXPECT_EQ(result.rows[0].begin()->second, "1");
}

// 测试插入与删除操作
TEST(MySQLPoolTest, InsertAndDeleteTest) {
    MySQLPool pool(
        dbConfig.pool_size,
        dbConfig.host,
        dbConfig.user,
        dbConfig.password,
        dbConfig.database,
        dbConfig.port
    );

    auto fut1 = pool.executeAsync("CREATE TABLE IF NOT EXISTS gtest_table(id INT PRIMARY KEY AUTO_INCREMENT, name VARCHAR(32))");
    EXPECT_TRUE(fut1.get().error.empty());

    auto fut2 = pool.executeAsync("INSERT INTO gtest_table(name) VALUES('Alice')");
    DbResult insertResult = fut2.get();
    EXPECT_TRUE(insertResult.error.empty());
    EXPECT_EQ(insertResult.affectedRows, 1);

    auto fut3 = pool.executeAsync("DELETE FROM gtest_table WHERE name='Alice'");
    DbResult deleteResult = fut3.get();
    EXPECT_TRUE(deleteResult.error.empty());
    EXPECT_EQ(deleteResult.affectedRows, 1);
}