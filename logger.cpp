#include <string>

#include "pch.h"
#include "logger.h"

void Logger::log(const std::string& msg) {
    _log += msg + '\n';
}

std::string Logger::getLog() {
    return _log;
}