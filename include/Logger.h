#pragma once
#include <boost/log/trivial.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <string>

class Logger {
public:
    static void init(const std::string& logFile, const std::string& level);
};