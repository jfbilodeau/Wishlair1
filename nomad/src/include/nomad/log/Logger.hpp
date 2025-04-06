//
// Created by jfbil on 2023-06-04.
//

#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "nomad/nomad.hpp"

namespace nomad {

enum class LogLevel {
    Debug = 1,
    Info = 2,
    Warning = 3,
    Error = 4,
    Fatal = 5,
};

struct LogEntry {
    std::chrono::system_clock::time_point time;
    LogLevel level;
    NomadString message;
};

class LogSink {
public:
    LogSink() = default;
    LogSink(const LogSink&) = delete;
    virtual ~LogSink() = default;

    // Allow sink to initialize itself before logs are flushed
    virtual void begin() { /* Override as necessary */ }

    virtual void log(const LogEntry* entry) = 0;

    // Signals the end of the log flush
    virtual void end() { /* Override as necessary */ }
};

class Logger {
public:
    explicit Logger(LogSink* sink);

    Logger(const Logger& other) = delete;

    ~Logger();

    void setLogLevel(LogLevel level);
    [[nodiscard]] LogLevel getLogLevel() const;

    void debug(const NomadString& message);
    void info(const NomadString& message);
    void warning(const NomadString& message);
    void error(const NomadString& message);
    void fatal(const NomadString& message);

    void addSink(LogSink* sink);
    void removeSink(LogSink* sink);

    void flush();

private:
    void log(LogLevel level, const NomadString& message);

    LogLevel m_logLevel;
    std::vector<LogSink*> m_sinks;
    std::vector<LogEntry> m_entries;
};

namespace log {

void debug(const NomadString& message);
void info(const NomadString& message);
void warning(const NomadString& message);
void error(const NomadString& message);
void fatal(const NomadString& message);

void flush();

} // namespace log

} // namespace nomad

