#ifndef LOGGER_H_4RWVDC5H
#define LOGGER_H_4RWVDC5H

#include <sstream>
#include <string>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/configurator.h>

template <typename... T>
void __LoggerDummyWrapper(T... t) {};

template <class T>
std::stringstream* __log_unpacker(std::stringstream* os, const T t) {
    *os << t;
    return os;
}

template <typename T, typename... Args>
std::string __log_buffer(T t, Args... data) {
    std::stringstream os;
    os << t;
    __LoggerDummyWrapper(__log_unpacker(&os, data)...);
    return std::move(os.str());
}

#define InitLog(config_file_name)                                       \
    do {                                                                \
        log4cplus::PropertyConfigurator::doConfigure(config_file_name); \
    } while (0)

#define DebugLogf(fmt, ...)                                                \
    do {                                                                   \
        log4cplus::Logger log = log4cplus::Logger::getInstance("program"); \
        LOG4CPLUS_DEBUG_FMT(log, fmt, __VA_ARGS__);                        \
    } while (0)

#define WarnLogf(fmt, ...)                                                 \
    do {                                                                   \
        log4cplus::Logger log = log4cplus::Logger::getInstance("program"); \
        LOG4CPLUS_WARN_FMT(log, fmt, __VA_ARGS__);                         \
    } while (0)

#define InfoLogf(fmt, ...)                                                 \
    do {                                                                   \
        log4cplus::Logger log = log4cplus::Logger::getInstance("program"); \
        LOG4CPLUS_INFO_FMT(log, fmt, __VA_ARGS__);                         \
    } while (0)

#define ErrorLogf(fmt, ...)                                                \
    do {                                                                   \
        log4cplus::Logger log = log4cplus::Logger::getInstance("program"); \
        LOG4CPLUS_ERROR_FMT(log, fmt, __VA_ARGS__);                        \
    } while (0)

#define BusinessLogf(fmt, ...)                                              \
    do {                                                                    \
        log4cplus::Logger log = log4cplus::Logger::getInstance("business"); \
        LOG4CPLUS_INFO_FMT(log, fmt, __VA_ARGS__);                          \
    } while (0)

#define AccessLogf(fmt, ...)                                              \
    do {                                                                  \
        log4cplus::Logger log = log4cplus::Logger::getInstance("access"); \
        LOG4CPLUS_INFO_FMT(log, fmt, __VA_ARGS__);                        \
    } while (0)

template <typename... Args>
void DebugLog(Args... args) {
    log4cplus::Logger log = log4cplus::Logger::getInstance("program");
    LOG4CPLUS_DEBUG(log, __log_buffer(std::forward<Args>(args)...));
}

template <typename... Args>
void WarnLog(Args... args) {
    log4cplus::Logger log = log4cplus::Logger::getInstance("program");
    LOG4CPLUS_WARN(log, __log_buffer(std::forward<Args>(args)...));
}

template <typename... Args>
void InfoLog(Args... args) {
    log4cplus::Logger log = log4cplus::Logger::getInstance("program");
    LOG4CPLUS_INFO(log, __log_buffer(std::forward<Args>(args)...));
}

template <typename... Args>
void BusinessLog(Args... args) {
    log4cplus::Logger log = log4cplus::Logger::getInstance("business");
    LOG4CPLUS_INFO(log, __log_buffer(std::forward<Args>(args)...));
}

template <typename... Args>
void AccessLog(Args... args) {
    log4cplus::Logger log = log4cplus::Logger::getInstance("access");
    LOG4CPLUS_INFO(log, __log_buffer(std::forward<Args>(args)...));
}

inline void InfoLogBody(char* dscr, unsigned char* data_p, int data_size) {
    char log_data[4096] = {0};
    int offset = 0;

    for (int index = 0; index < data_size; index++) {
        if (offset > 4000) {
            break;
        }
        if (((index % 8) == 0) && (index != 0)) {
            offset += sprintf(log_data + offset, "\r\n");
        }
        offset += sprintf(log_data + offset, "%02x ", data_p[index]);
    }

    InfoLogf("%s: \r\n%s", dscr, log_data);
}
#endif /* end of include guard: LOGGER_H_4RWVDC5H */
