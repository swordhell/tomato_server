#include "Server.h"
#include "Session.h"
#include <memory>

Server::Server(boost::asio::io_context &io_context, short port,
               LogicProcessor &logic, MySQLPool &db)
        : acceptor_(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
          logic_(logic), db_(db) {
    do_accept();
}

void Server::do_accept() {
    acceptor_.async_accept([this](boost::system::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (!ec) {
            std::make_shared<Session>(std::move(socket), logic_, db_)->start();
        }
        do_accept();
    });
}