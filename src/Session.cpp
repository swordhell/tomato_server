#include "Session.h"
#include "ProtocolParser.h"
#include <boost/log/trivial.hpp>

Session::Session(boost::asio::ip::tcp::socket socket, LogicProcessor &logic, MySQLPool &db)
    : socket_(std::move(socket)), logic_(logic), db_(db) {}

void Session::start() { do_read(); }

void Session::do_read() {
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(data_, sizeof(data_)),
                            [this, self](boost::system::error_code ec, std::size_t length) {
        if (!ec) {
            std::vector<uint8_t> data;
            ProtocolParser parser;
            parser.appendData(data);

            // 解析协议
            auto message = parser.parse();

            // 提交到逻辑线程
            logic_.post([this, self, message]() {
                // 查询数据库
                auto fut = db_.executeAsync("SELECT 'Hello from MySQL'");
                DbResult db_result = fut.get();

                if (!db_result.error.empty()) {
                    BOOST_LOG_TRIVIAL(error) << "DB Error: " << db_result.error;
                } else if (db_result.isQuery) {
                    for (auto &row : db_result.rows) {
                        BOOST_LOG_TRIVIAL(info) << "id=" << row["id"]
                                                << ", name=" << row["name"]
                                                << ", value=" << row["value"];
                    }
                } else {
                    BOOST_LOG_TRIVIAL(info) << "Affected rows: " << db_result.affectedRows;
                }

                // 可以根据解析后的 message 决定是否回应客户端
                do_write("Response from server");
            });

            do_read();
        } else {
            BOOST_LOG_TRIVIAL(info) << "Session closed: " << ec.message();
            socket_.close();
        }
    });
}

void Session::do_write(const std::string &msg) {
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(msg),
                             [this, self](boost::system::error_code ec, std::size_t) {
        if (ec) {
            BOOST_LOG_TRIVIAL(error) << "Write error: " << ec.message();
            socket_.close();
        }
    });
}