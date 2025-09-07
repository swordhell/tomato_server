#include "Session.h"
#include <boost/asio/post.hpp>

Session::Session(boost::asio::ip::tcp::socket socket, LogicProcessor &logic, MySQLPool &db)
        : socket_(std::move(socket)), logic_(logic), db_(db) {}

void Session::start() { do_read(); }

void Session::do_read() {
    auto self = shared_from_this();
    socket_.async_read_some(boost::asio::buffer(data_, sizeof(data_)),
                            [this, self](boost::system::error_code ec, std::size_t length) {
                                if (!ec) {
                                    std::string req(data_, length);

                                    // 交给逻辑线程处理
                                    logic_.post([this, self, req]() {
                                        auto fut = db_.queryAsync("SELECT 'Hello from MySQL'");
                                        std::string db_result = fut.get();
                                        std::string response = "Request: " + req + ", DB: " + db_result;

                                        boost::asio::post(socket_.get_executor(), [this, self, response]() {
                                            do_write(response);
                                        });
                                    });

                                    do_read();
                                }
                            });
}

void Session::do_write(const std::string &msg) {
    auto self = shared_from_this();
    boost::asio::async_write(socket_, boost::asio::buffer(msg),
                             [this, self](boost::system::error_code ec, std::size_t /*length*/) {
                                 if (ec) socket_.close();
                             });
}