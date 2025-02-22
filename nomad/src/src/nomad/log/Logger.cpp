//
// Created by jfbil on 2023-06-04.
//

#include "nomad/log/Logger.hpp"

#include "nomad/log/ConsoleSink.hpp"

#include "SDL.h"

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory_resource>
#include <ctime>

namespace nomad {

Logger::Logger(LogSink* sink) {
    add_sink(sink);

    info("Logger initialized.");
}

Logger::~Logger() {
    info("Logger shutting down.");

    flush();
}

void Logger::set_log_level(LogLevel level) {
    m_log_level = level;
}

LogLevel Logger::get_log_level() const {
    return m_log_level;
}

void Logger::debug(const NomadString& message) {
    log(LogLevel::Debug, message);
}


void Logger::info(const NomadString& message) {
    log(LogLevel::Info, message);
}

void Logger::warning(const NomadString& message) {
    log(LogLevel::Warning, message);
}

void Logger::error(const NomadString& message) {
    log(LogLevel::Error, message);

    // Make sure the error is logged immediately.
    flush();
}

void Logger::fatal(const NomadString& message) {
    log(LogLevel::Fatal, message);

    // Make sure the error is logged immediately.
    flush();

    exit(1);
}

void Logger::add_sink(LogSink* sink) {
    m_sinks.push_back(sink);
}

void Logger::remove_sink(LogSink* sink) {
    m_sinks.erase(std::remove(m_sinks.begin(), m_sinks.end(), sink), m_sinks.end());
}

void Logger::flush() {
    for (auto& sink: m_sinks) {
        sink->begin();

        for (auto& entry: m_entries) {
            sink->log(&entry);
        }

        sink->end();
    }

    m_entries.clear();

}

void Logger::log(LogLevel level, const NomadString& message) {
    if (level < m_log_level) {
        // Skip!
        return;
    }

    NomadString log_level_name;

    switch (level) {
        case LogLevel::Debug:
            log_level_name = "DEBUG";
            break;
        case LogLevel::Info:
            log_level_name = "INFO";
            break;
        case LogLevel::Warning:
            log_level_name = "WARNING";
            break;
        case LogLevel::Error:
            log_level_name = "ERROR";
            break;
        case LogLevel::Fatal:
            log_level_name = "FATAL";
            break;
        default:
            log_level_name = "UNKNOWN";
            break;
    }

    // time_t time = std::time(nullptr);
    // std::stringstream ss;
    // tm tm;
    //
    // gmtime(&time);
    //
    // ss
    //     << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S")
    //     << " ["
    //     << log_level_name
    //     << "] "
    //     << message;
    auto time = std::chrono::system_clock::now();

    auto line = std::format("{0:%F}T{0:%T} [{1}] {2}", time, log_level_name, message);

    m_entries.emplace_back(LogEntry{
        time,
        level,
        line
    });
}

namespace log {

// Global logger:
auto logger = new Logger(new ConsoleSink());

void debug(const NomadString& message) {
    logger->debug(message);
}

void info(const NomadString& message) {
    logger->info(message);
}

void warning(const NomadString& message) {
    logger->warning(message);
}

void error(const NomadString& message) {
    logger->error(message);
}

void fatal(const NomadString& message) {
    logger->fatal(message);
}

void flush() {
    logger->flush();
}

} // log

} // nomad
