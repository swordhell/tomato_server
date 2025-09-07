#pragma once
#include <boost/asio.hpp>
#include <memory>
#include <string>
#include "LogicProcessor.h"
#include "MySQLPool.h"

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::ip::tcp::socket socket, LogicProcessor& logic, MySQLPool& db);
    void start();

private:
    void do_read();
    void do_write(const std::string& msg);

    boost::asio::ip::tcp::socket socket_;
    char data_[1024];
    LogicProcessor& logic_;
    MySQLPool& db_;
};