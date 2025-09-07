#include "Logger.h"
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

static logging::trivial::severity_level parseLevel(const std::string& level) {
    if (level == "trace") return logging::trivial::trace;
    if (level == "debug") return logging::trivial::debug;
    if (level == "info")  return logging::trivial::info;
    if (level == "warning") return logging::trivial::warning;
    if (level == "error") return logging::trivial::error;
    if (level == "fatal") return logging::trivial::fatal;
    return logging::trivial::info;
}

void Logger::init(const std::string& logFile, const std::string& level) {
    logging::add_file_log(
            keywords::file_name = logFile,
            keywords::rotation_size = 10 * 1024 * 1024, // 10MB
            keywords::format = "[%TimeStamp%] [%Severity%]: %Message%"
    );

    logging::add_console_log(
            std::clog,
            keywords::format = "[%TimeStamp%] [%Severity%]: %Message%"
    );

    logging::add_common_attributes();
    logging::core::get()->set_filter(
            logging::trivial::severity >= parseLevel(level)
    );
}