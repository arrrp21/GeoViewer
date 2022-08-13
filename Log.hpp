#pragma once

#include "fmt/format.h"
#include <QDebug>

#define LOG(logging_level, ...) log(__FILE__, __LINE__, logging_level, __VA_ARGS__)
#define LOG_INFO(...) LOG("[INFO]", __VA_ARGS__)
#define LOG_WRN(...) LOG("[WRN]", __VA_ARGS__)
#define LOG_ERR(...) LOG("[ERR]", __VA_ARGS__)

template <typename ... Args>
void log(const char* path, int line, const char* loggingLevel, const char* format, Args&&... args)
{
    std::string pathStr{path};
    std::string fileName = pathStr.substr(pathStr.find_last_of("/\\") + 1);
    std::string prefix = fmt::format("{}:{} {} ", fileName, line, loggingLevel);
    std::string str = fmt::format(format, std::forward<Args>(args)...);
    qDebug() << (prefix + str).c_str();
}
