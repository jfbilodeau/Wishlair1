//
// Created by jfbil on 2023-09-27.
//

#include "nomad/log/MemorySink.hpp"

namespace nomad {

void MemorySink::log(const LogEntry* entry) {
    m_entries.push_back(*entry);
}

const std::vector<LogEntry>& MemorySink::get_entries() const {
    return m_entries;
}

void MemorySink::clear() {
    m_entries.clear();
}

} // nomad