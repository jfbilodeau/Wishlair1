//
// Created by jfbil on 2023-06-04.
//

#pragma once

#include "nomad/log/Logger.hpp"

namespace nomad {

    class ConsoleSink : public LogSink {
    public:
        ConsoleSink() = default;
        ConsoleSink(const ConsoleSink&) = delete;
        ~ConsoleSink() override = default;

        void log(const LogEntry* entry) override;
        void end() override; // Flushes the console
    };

} // nomad

