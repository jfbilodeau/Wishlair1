//
// Created by jfbil on 2023-07-06.
//

#ifndef NOMAD_EVENT_HPP
#define NOMAD_EVENT_HPP

#include "nomad/Nomad.hpp"

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

    void add_event(const NomadString& name, NomadId script_id);
    void remove_event(const NomadString& name);

    NomadId get_script_id_for_event(const NomadString& name) const;

private:
    std::vector<Event> m_events;
};

} // nomad

#endif //NOMAD_EVENT_HPP
