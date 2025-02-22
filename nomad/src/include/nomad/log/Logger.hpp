//
// Created by jfbil on 2023-06-04.
//

#ifndef NOMAD_LOGGER_HPP
#define NOMAD_LOGGER_HPP

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

    void set_log_level(LogLevel level);
    [[nodiscard]] LogLevel get_log_level() const;

    void debug(const NomadString& message);
    void info(const NomadString& message);
    void warning(const NomadString& message);
    void error(const NomadString& message);
    void fatal(const NomadString& message);

    void add_sink(LogSink* sink);
    void remove_sink(LogSink* sink);

    void flush();

private:
    void log(LogLevel level, const NomadString& message);

    LogLevel m_log_level;
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

#endif //NOMAD_LOGGER_HPP
