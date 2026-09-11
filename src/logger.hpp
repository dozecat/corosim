#pragma once

#include <cstdio>
#include <source_location>
#include <string_view>

namespace corosim {

enum class LogLevel {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR_,
    FATAL,
};

namespace detail {

constexpr std::string_view level_name(LogLevel lv) {
    switch (lv) {
    case LogLevel::TRACE:  return "TRACE";
    case LogLevel::DEBUG:  return "DEBUG";
    case LogLevel::INFO:   return "INFO";
    case LogLevel::WARN:   return "WARN";
    case LogLevel::ERROR_: return "ERROR";
    case LogLevel::FATAL:  return "FATAL";
    }
    return "?";
}

inline LogLevel& min_level() {
    static LogLevel lv = LogLevel::TRACE;
    return lv;
}

inline void log_write(LogLevel lv, std::string_view msg,
                      std::source_location loc = std::source_location::current()) {
    if (lv < min_level()) {
        return;
    }
    std::fprintf(stderr, "[%s] %s:%d: %s\n",
                 level_name(lv).data(),
                 loc.file_name(), loc.line(),
                 msg.data());
}

} // namespace detail

inline void set_log_level(LogLevel lv) { detail::min_level() = lv; }

} // namespace corosim

#define COROSIM_LOG(lv, msg)                                             \
    do {                                                                 \
        ::corosim::detail::log_write(::corosim::LogLevel::lv, msg);     \
    } while (false)

#define COROSIM_TRACE(msg) COROSIM_LOG(TRACE, msg)
#define COROSIM_DEBUG(msg) COROSIM_LOG(DEBUG, msg)
#define COROSIM_INFO(msg)  COROSIM_LOG(INFO, msg)
#define COROSIM_WARN(msg)  COROSIM_LOG(WARN, msg)
#define COROSIM_ERROR(msg) COROSIM_LOG(ERROR_, msg)
#define COROSIM_FATAL(msg) COROSIM_LOG(FATAL, msg)
