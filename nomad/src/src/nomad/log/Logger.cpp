//
// Created by jfbil on 2023-06-04.
//

#include "nomad/log/Logger.hpp"

#include "nomad/log/ConsoleSink.hpp"

#include "SDL.h"

#include <algorithm>
#include <chrono>
#include <memory_resource>
#include <ctime>

namespace nomad {

Logger::Logger(LogSink* sink) {
    addSink(sink);

    info("Logger initialized.");
}

Logger::~Logger() {
    info("Logger shutting down.");

    flush();
}

void Logger::setLogLevel(LogLevel level) {
    m_logLevel = level;
}

LogLevel Logger::getLogLevel() const {
    return m_logLevel;
}

void Logger::debug(const NomadString& message, const Location& location) {
    log(LogLevel::Debug, message, location);
}


void Logger::info(const NomadString& message, const Location& location) {
    log(LogLevel::Info, message, location);
}

void Logger::warning(const NomadString& message, const Location& location) {
    log(LogLevel::Warning, message, location);
}

void Logger::error(const NomadString& message, const Location& location) {
    log(LogLevel::Error, message, location);

    // Make sure the error is logged immediately.
    flush();
}

void Logger::fatal(const NomadString& message, const Location& location) {
    log(LogLevel::Fatal, message, location);

    // Make sure the error is logged immediately.
    flush();

    exit(1);
}

void Logger::addSink(LogSink* sink) {
    m_sinks.push_back(sink);
}

void Logger::removeSink(LogSink* sink) {
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

void Logger::log(LogLevel level, const NomadString& message, const Location& location) {
    if (level < m_logLevel) {
        // Skip!
        return;
    }

    NomadString logLevelName;

    switch (level) {
        case LogLevel::Debug:
            logLevelName = "DEBUG";
            break;
        case LogLevel::Info:
            logLevelName = "INFO";
            break;
        case LogLevel::Warning:
            logLevelName = "WARNING";
            break;
        case LogLevel::Error:
            logLevelName = "ERROR";
            break;
        case LogLevel::Fatal:
            logLevelName = "FATAL";
            break;
        default:
            logLevelName = "UNKNOWN";
            break;
    }

    auto time = std::chrono::system_clock::now();

    auto line = std::format(
        "{0:%F}T{0:%T} [{1}] [{2}] [{3}:{4}:{5}] {6}",
        time,
        logLevelName,
        location.function,
        location.file,
        location.line,
        location.column,
        message
    );

    m_entries.emplace_back(LogEntry{
        time,
        level,
        line
    });
}

namespace log {

// Global logger:
auto logger = new Logger(new ConsoleSink());

void debug(const NomadString& message, const Location& location) {
    logger->debug(message, location);
}

void info(const NomadString& message, const Location& location) {
    logger->info(message, location);
}

void warning(const NomadString& message, const Location& location) {
    logger->warning(message, location);
}

void error(const NomadString& message, const Location& location) {
    logger->error(message, location);
}

void fatal(const NomadString& message, const Location& location) {
    logger->fatal(message, location);
}

void flush() {
    logger->flush();
}

} // log

} // nomad
