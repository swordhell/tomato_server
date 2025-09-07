#include <boost/asio.hpp>
#include "Server.h"
#include "LogicProcessor.h"
#include "MySQLPool.h"
#include "ConfigLoader.h"
#include "Logger.h"
#include <iostream>

int main() {
    try {
        auto cfg = ConfigLoader::load("config.yaml");

        Logger::init(cfg.log.file, cfg.log.level);

        BOOST_LOG_TRIVIAL(info) << "Tomato Server starting...";
        BOOST_LOG_TRIVIAL(debug) << "Connecting to DB: " << cfg.db.host;

        BOOST_LOG_TRIVIAL(info) << "Server running...";
        boost::asio::io_context io_context;

        MySQLPool db(cfg.db.pool_size, cfg.db.host, cfg.db.user, cfg.db.password, cfg.db.name);

        LogicProcessor logic(4);

        Server server(io_context, 12345, logic, db);

        BOOST_LOG_TRIVIAL(info)  << "Server running on port 12345...\n";
        io_context.run();
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}