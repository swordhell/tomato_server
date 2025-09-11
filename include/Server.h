#pragma once
#include <boost/asio.hpp>
#include "LogicProcessor.h"
#include "MySQLPool.h"

class Server {
public:
    Server(boost::asio::io_context& io_context, const std::uint16_t port,
           LogicProcessor& logic, MySQLPool& db);

private:
    void do_accept();

    boost::asio::ip::tcp::acceptor acceptor_;
    LogicProcessor& logic_;
    MySQLPool& db_;
};