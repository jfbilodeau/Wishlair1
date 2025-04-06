//
// Created by jfbil on 2023-07-06.
//

#pragma once

#include "nomad/nomad.hpp"

#include <vector>

namespace nomad {

class Event {
public:
    Event(const NomadString& name, NomadId script_id);
    Event(const Event&) = default;
    ~Event() = default;

    [[nodiscard]] NomadString get_name() const;;
    [[nodiscard]] NomadId get_script_id() const;;

private:
    NomadString m_name;
    NomadId m_script_id;
};

class EventManager {
public:
    EventManager() = default;
    EventManager(const EventManager&) = delete;
    ~EventManager() = default;

    void addEvent(const NomadString& name, NomadId scriptId);
    void removeEvent(const NomadString& name);

    [[nodiscard]]
    NomadId getScriptIdForEvent(const NomadString& name) const;

private:
    std::vector<Event> m_events;
};

} // nomad
